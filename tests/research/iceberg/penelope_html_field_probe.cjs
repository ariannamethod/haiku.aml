const fs = require('fs');
const vm = require('vm');
if (process.argv.length !== 3) {
  console.error('usage: node penelope_html_field_probe.cjs PENELOPE_HTML_PATH');
  process.exit(2);
}
const html = fs.readFileSync(process.argv[2], 'utf8');
const script = html.split('<script>')[1];
if (!script || !script.includes('const weightFileInput =')) {
  throw new Error('expected pinned Penelope script and weightFileInput boundary');
}
const body = script.split('const weightFileInput =')[0];
const elements = new Map();
const context = {
  console,
  setTimeout: fn => { fn(); return 0; },
  document: {getElementById: id => {
    if (!elements.has(id)) elements.set(id, {innerHTML:'',textContent:'',style:{},classList:{add(){},remove(){}}});
    return elements.get(id);
  }},
};
vm.createContext(context);
vm.runInContext(body + `
let probeSeed = 42;
Math.random = () => { probeSeed = (Math.imul(probeSeed,1664525) + 1013904223) >>> 0; return probeSeed / 4294967296; };
const probeWords = [];
showWord = async (_, word) => { probeWords.push(word); };
sleep = async () => {};
forward = () => { throw new Error('unexpected neural forward in field mode'); };
runChain('darkness eats the city').then(() => console.log(JSON.stringify({
  mode: model === null ? 'field' : 'trained',
  rng: 'probe LCG32 seed42',
  words: probeWords,
  metrics: document.getElementById('metrics').innerHTML,
  field: {chambers, destiny, trauma, cooc: cooc.size, bigrams: bigrams.size}
}, null, 2)));
`, context);
