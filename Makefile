AML_ROOT ?= ../ariannamethod.ai
AML ?= $(AML_ROOT)/runner/aml
AMLC ?= $(AML_ROOT)/tools/amlc
AML_LIB ?= $(AML_ROOT)/libaml.a

.PHONY: test test-numerical test-text test-lexical

test: test-numerical test-text test-lexical

test-numerical:
	@HAIKU_AML="$(abspath $(AML))" HAIKU_AMLC="$(abspath $(AMLC))" HAIKU_AML_LIB="$(abspath $(AML_LIB))" bash tests/run_numerical.sh

test-text:
	@HAIKU_AML="$(abspath $(AML))" HAIKU_AMLC="$(abspath $(AMLC))" HAIKU_AML_LIB="$(abspath $(AML_LIB))" bash tests/run_text.sh

test-lexical:
	@HAIKU_AML="$(abspath $(AML))" HAIKU_AMLC="$(abspath $(AMLC))" HAIKU_AML_LIB="$(abspath $(AML_LIB))" bash tests/run_lexical.sh
