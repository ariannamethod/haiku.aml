import argparse
from pathlib import Path

parser = argparse.ArgumentParser(description='Generate the bounded PEN7 / NoTorch parity probe.')
parser.add_argument('--source-root', type=Path, default=Path.cwd(),
                    help='Directory containing the pinned 1984, janus, and notorch checkouts (default: cwd).')
parser.add_argument('--output', type=Path, default=Path.cwd() / 'pen7_notorch_probe.c',
                    help='Generated C path (default: ./pen7_notorch_probe.c).')
args = parser.parse_args()
source_root = args.source_root.resolve()
source = (source_root / '1984/penelope.c').read_text()
start = source.index('static void forward(Model *m, int *bpe_ids, int seq_len, float *logits) {')
end = source.index('\n/* ═', start)
body = source[start:end].replace('static void forward(', 'static void forward_nt(')
body = body.replace('rmsnorm(', 'probe_norm(').replace('matmul_mv(', 'probe_mv(').replace('apply_rope(', 'probe_rope(')
a = body.index('        /* 5. Multi-head causal attention:')
b = body.index('        /* av is [S, DIM]', a)
body = body[:a] + '        probe_attention(av, q, k, v, S);\n\n' + body[b:]
body = body.replace('    float *att = (float *)malloc(S * S * N_HEADS * sizeof(float));  /* per-head attention scores */\n', '')
body = body.replace('free(att); ', '')
prefix = r'''
/* Scratch PEN7 audit only. Architecture scaffold is extracted verbatim from
 * pinned penelope.c, with its math calls redirected to canonical NoTorch.
 * No optimizer, global tape, mutable model state, or product changes. */
#define main penelope_reference_cli_main
#define rmsnorm penelope_reference_rmsnorm
#include "1984/penelope.c"
#undef rmsnorm
#undef main
#include "notorch/notorch.h"
#include "notorch/gguf.h"
#include "notorch/harness/runtime.h"
#include <stdint.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <fcntl.h>
#include <unistd.h>

static void probe_mv(float *w, float *x, float *out, int rows, int cols) {
    nt_blas_matvec(out, w, x, rows, cols);
}
static void probe_norm(float *x, float *g, float *out, int n) {
    rmsnorm(out, x, g, n, 1e-5f);
}
static void probe_rope(float *q, float *k, int s, int heads, int hd) {
    int neox = getenv("PEN7_WRONG_ROPE") ? 0 : 1;
    for (int t=0; t<s; t++) for (int h=0; h<heads; h++) {
        rope(q+(t*heads+h)*hd, t, hd, 10000.f, neox);
        rope(k+(t*heads+h)*hd, t, hd, 10000.f, neox);
    }
}
static void probe_attention(float *out, float *q, float *k, float *v, int s) {
    /* NoTorch forward-only attention accepts contiguous heads. Gather one
     * head, then ask each query to attend exactly its causal prefix. */
    float kh[MAX_SEQ*HEAD_DIM], vh[MAX_SEQ*HEAD_DIM];
    for (int h=0; h<N_HEADS; h++) {
        for (int t=0; t<s; t++) {
            memcpy(kh+t*HEAD_DIM, k+t*DIM+h*HEAD_DIM, HEAD_DIM*sizeof(float));
            memcpy(vh+t*HEAD_DIM, v+t*DIM+h*HEAD_DIM, HEAD_DIM*sizeof(float));
        }
        for (int t=0; t<s; t++) {
            int rc=nt_attention(out+t*DIM+h*HEAD_DIM, q+t*DIM+h*HEAD_DIM,
                                kh, vh, 1, t+1, HEAD_DIM);
            if (rc) { fprintf(stderr,"attention failed\n"); exit(3); }
        }
    }
}
'''
suffix = r'''
typedef struct { Model model; void *base; size_t bytes; } Mapped;
static Mapped bind_bytes(void *base,size_t bytes) {
    Mapped m={0}; m.base=base; m.bytes=bytes;
    int expected[8]={0x50454E37,BPE_VOCAB,NWORDS,DIM,HDIM,N_HEADS,N_LAYERS,MAX_SEQ};
    if(memcmp(m.base,expected,32)) { fprintf(stderr,"bad PEN7 header\n"); exit(3); }
    float *p=(float*)((unsigned char*)m.base+32);
    #define TAKE(field, count) do { m.model.field=p; p+=(count); } while(0)
    TAKE(tok_emb, BPE_VOCAB*DIM); TAKE(pos_emb, MAX_SEQ*DIM);
    TAKE(final_norm,DIM); TAKE(lm_head,BPE_VOCAB*DIM);
    for(int l=0;l<N_LAYERS;l++) {
        TAKE(layers[l].attn_norm,DIM);
        TAKE(layers[l].wq,DIM*DIM); TAKE(layers[l].wk,DIM*DIM);
        TAKE(layers[l].wv,DIM*DIM); TAKE(layers[l].wo,DIM*DIM);
        TAKE(layers[l].wr,DIM*DIM);
        memcpy(m.model.layers[l].gate,p,2*sizeof(float)); p+=2;
        TAKE(layers[l].ffn_norm,DIM); TAKE(layers[l].w_gate,HDIM*DIM);
        TAKE(layers[l].w_up,HDIM*DIM); TAKE(layers[l].w_down,DIM*HDIM);
    }
    #undef TAKE
    if((unsigned char*)p!=(unsigned char*)m.base+m.bytes) exit(3);
    return m;
}
static Mapped load_mapped(const char *path) {
    struct stat st; int fd=open(path,O_RDONLY);
    if(fd<0 || fstat(fd,&st) || st.st_size!=32+(off_t)total_param_count()*4) {
        fprintf(stderr,"bad PEN7 size/path\n"); exit(3);
    }
    size_t bytes=(size_t)st.st_size;
    void *base=mmap(NULL,bytes,PROT_READ,MAP_PRIVATE,fd,0); close(fd);
    if(base==MAP_FAILED) { perror("mmap"); exit(3); }
    return bind_bytes(base,bytes);
}
static long rss_kib(void) {
    FILE *f=fopen("/proc/self/status","r"); char line[256]; long rss=-1;
    if(f) { while(fgets(line,sizeof(line),f)) if(sscanf(line,"VmRSS: %ld",&rss)==1) break; fclose(f); }
    return rss;
}
static int argmax(const float *x) { int b=0; for(int i=1;i<BPE_VOCAB;i++) if(x[i]>x[b]) b=i; return b; }
static uint64_t tape_identity(void) {
    const unsigned char *p=(const unsigned char*)nt_tape_get();
    uint64_t h=UINT64_C(14695981039346656037);
    for(size_t i=0;i<sizeof(nt_tape);i++) { h^=p[i]; h*=UINT64_C(1099511628211); }
    return h;
}
static void print_logits(const float *x) {
    putchar('[');
    for(int i=0;i<BPE_VOCAB;i++) printf("%s%.9g",i?",":"",x[i]);
    putchar(']');
}
static int blend_probe(const char *a_path,const char *b_path) {
    const char *prompt="memory turns through the silence";
    int ids[MAX_SEQ]; int n=bpe_encode(prompt,ids,MAX_SEQ);
    uint64_t tape_before=tape_identity();
    long baseline_rss=rss_kib();
    Mapped a=load_mapped(a_path), b=load_mapped(b_path);
    void *mix_bytes=malloc(a.bytes);
    if(!mix_bytes) { fprintf(stderr,"midpoint allocation failed\n"); return 3; }
    memcpy(mix_bytes,a.base,32);
    const float *wa=(const float*)((const unsigned char*)a.base+32);
    const float *wb=(const float*)((const unsigned char*)b.base+32);
    float *wm=(float*)((unsigned char*)mix_bytes+32);
    int nonfinite_weights=0;
    for(int i=0;i<total_param_count();i++) {
        wm[i]=0.5f*wa[i]+0.5f*wb[i];
        if(!isfinite(wm[i])) nonfinite_weights++;
    }
    Mapped mid=bind_bytes(mix_bytes,a.bytes);
    long blended_rss=rss_kib();
    float la[BPE_VOCAB],lb[BPE_VOCAB],ref[BPE_VOCAB],nt[BPE_VOCAB];
    forward_nt(&a.model,ids,n,la); forward_nt(&b.model,ids,n,lb);
    double t=now_ms(); forward(&mid.model,ids,n,ref); double ref_ms=now_ms()-t;
    t=now_ms(); forward_nt(&mid.model,ids,n,nt); double nt_ms=now_ms()-t;
    double maxabs=0,sumerr2=0,mae_avg=0,maxabs_avg=0;
    int changed=0,nonfinite=0,differing_from_average=0;
    float avg[BPE_VOCAB];
    for(int i=0;i<BPE_VOCAB;i++) {
        double err=fabs((double)ref[i]-nt[i]); if(err>maxabs) maxabs=err; sumerr2+=err*err;
        if(memcmp(ref+i,nt+i,sizeof(float))) changed++;
        if(!isfinite(la[i]) || !isfinite(lb[i]) || !isfinite(ref[i]) || !isfinite(nt[i])) nonfinite++;
        avg[i]=0.5f*la[i]+0.5f*lb[i];
        double diff=fabs((double)nt[i]-avg[i]); mae_avg+=diff;
        if(diff>maxabs_avg) maxabs_avg=diff;
        if(memcmp(nt+i,avg+i,sizeof(float))) differing_from_average++;
    }
    int tape_unchanged=tape_before==tape_identity();
    int failed=nonfinite_weights || nonfinite || maxabs>1e-5 || argmax(ref)!=argmax(nt) ||
               !tape_unchanged || nt_tape_get()->n_params || nt_tape_get()->active || nt_tape_get()->count;
    struct rusage usage; getrusage(RUSAGE_SELF,&usage);
    printf("{\"alpha\":0.5,\"mix_rule\":\"float32(0.5f*A[i]+0.5f*B[i]) for every parameter\",\"prompt\":\"%s\",\"ids\":[",prompt);
    for(int i=0;i<n;i++) printf("%s%d",i?",":"",ids[i]);
    printf("],\"parameters_mixed\":%d,\"extra_body_bytes\":%zu,\"nonfinite_weights\":%d,\"nonfinite_logits\":%d,\"reference_ms\":%.6f,\"notorch_ms\":%.6f,\"changed_f32\":%d,\"max_abs\":%.12g,\"rms\":%.12g,\"reference_argmax\":%d,\"notorch_argmax\":%d,\"a_argmax\":%d,\"b_argmax\":%d,\"averaged_logits_argmax\":%d,\"differing_from_averaged_logits\":%d,\"max_abs_from_averaged_logits\":%.12g,\"mae_from_averaged_logits\":%.12g,\"baseline_rss_kib\":%ld,\"blended_rss_kib\":%ld,\"final_rss_kib\":%ld,\"peak_rss_kib\":%ld,\"tape_unchanged\":%s,\"optimizer_slots\":%d,\"status\":\"%s\",\"logits_a\":",
        total_param_count(),mid.bytes,nonfinite_weights,nonfinite,ref_ms,nt_ms,changed,maxabs,sqrt(sumerr2/BPE_VOCAB),argmax(ref),argmax(nt),argmax(la),argmax(lb),argmax(avg),differing_from_average,maxabs_avg,mae_avg/BPE_VOCAB,baseline_rss,blended_rss,rss_kib(),usage.ru_maxrss,tape_unchanged?"true":"false",nt_tape_get()->n_params,failed?"FAIL":"PASS");
    print_logits(la); printf(",\"logits_b\":"); print_logits(lb);
    printf(",\"logits_midpoint\":"); print_logits(nt); printf("}\n");
    munmap(a.base,a.bytes); munmap(b.base,b.bytes); free(mid.base);
    return failed;
}
int main(int argc,char **argv) {
    if(argc==4 && !strcmp(argv[1],"--blend")) return blend_probe(argv[2],argv[3]);
    if(argc!=3) return 2;
    if(!strcmp(argv[1],"--loaders")) {
        int count=-1; nt_tensor **t=nt_load(argv[2],&count);
        gguf_file *g=gguf_open(argv[2]);
        printf("{\"nt_load_accepted\":%s,\"nt_load_count\":%d,\"gguf_accepted\":%s}\n",
                t?"true":"false",count,g?"true":"false");
        if(t) { for(int i=0;i<count;i++) nt_tensor_free(t[i]); free(t); }
        if(g) gguf_close(g);
        return (t||g)?1:0;
    }
    const char *prompt="memory turns through the silence";
    int ids[MAX_SEQ]; int n=bpe_encode(prompt,ids,MAX_SEQ);
    uint64_t tape_before=tape_identity();
    long base_rss=rss_kib();
    Mapped models[2]={load_mapped(argv[1]),load_mapped(argv[2])};
    printf("{\"prompt\":\"%s\",\"ids\":[",prompt);
    for(int i=0;i<n;i++) printf("%s%d",i?",":"",ids[i]);
    printf("],\"params_each\":%d,\"raw_bytes_each\":%zu,\"baseline_rss_kib\":%ld,\"mapped_rss_kib\":%ld,\"runs\":[",
            total_param_count(),models[0].bytes,base_rss,rss_kib());
    int failed=0;
    for(int m=0;m<2;m++) {
        float ref[BPE_VOCAB],nt[BPE_VOCAB];
        double t=now_ms(); forward(&models[m].model,ids,n,ref); double ref_ms=now_ms()-t;
        t=now_ms(); forward_nt(&models[m].model,ids,n,nt); double nt_ms=now_ms()-t;
        double maxabs=0,rms=0; int nonfinite=0,changed=0;
        for(int i=0;i<BPE_VOCAB;i++) {
            double d=fabs((double)ref[i]-nt[i]); if(d>maxabs) maxabs=d; rms+=d*d;
            if(!isfinite(ref[i]) || !isfinite(nt[i])) nonfinite++;
            if(memcmp(ref+i,nt+i,sizeof(float))) changed++;
        }
        if(nonfinite || maxabs>1e-5 || argmax(ref)!=argmax(nt)) failed=1;
        printf("%s{\"model\":\"%s\",\"reference_ms\":%.6f,\"notorch_ms\":%.6f,\"max_abs\":%.12g,\"rms\":%.12g,\"changed_f32\":%d,\"nonfinite\":%d,\"reference_argmax\":%d,\"notorch_argmax\":%d,\"rss_kib\":%ld}",
               m?",":"",argv[m+1],ref_ms,nt_ms,maxabs,sqrt(rms/BPE_VOCAB),changed,nonfinite,argmax(ref),argmax(nt),rss_kib());
    }
    struct rusage usage; getrusage(RUSAGE_SELF,&usage);
    int tape_unchanged=tape_before==tape_identity();
    int optimizer_slots=nt_tape_get()->n_params;
    if(!tape_unchanged || optimizer_slots || nt_tape_get()->active || nt_tape_get()->count) failed=1;
    printf("],\"peak_rss_kib\":%ld,\"tape_unchanged\":%s,\"optimizer_slots\":%d,\"status\":\"%s\"}\n",
            usage.ru_maxrss,tape_unchanged?"true":"false",optimizer_slots,failed?"FAIL":"PASS");
    for(int m=0;m<2;m++) munmap(models[m].base,models[m].bytes);
    return failed;
}
'''
args.output.write_text(prefix + body + suffix)
