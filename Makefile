AML_ROOT ?= ../ariannamethod.ai
AML ?= $(AML_ROOT)/runner/aml-notorch
AMLC ?= $(AML_ROOT)/tools/amlc
AML_LIB ?= $(AML_ROOT)/libaml.a
AML_INCLUDE ?= $(AML_ROOT)/core
AML_BRIDGE_LIB ?= $(AML_ROOT)/libaml_notorch.a
NOTORCH_ROOT ?= ../notorch
NOTORCH_LIB ?= $(NOTORCH_ROOT)/libnotorch.a

.PHONY: test test-numerical test-text test-lexical test-cloud test-form test-generator test-mathbrain test-rae test-tokenizer test-tokenizer-setup test-foreground test-state

test: test-numerical test-text test-lexical test-cloud test-form test-generator test-mathbrain test-rae test-tokenizer test-foreground test-state

test-numerical:
	@HAIKU_AML="$(abspath $(AML))" HAIKU_AMLC="$(abspath $(AMLC))" HAIKU_AML_LIB="$(abspath $(AML_LIB))" bash tests/run_numerical.sh

test-text:
	@HAIKU_AML="$(abspath $(AML))" HAIKU_AMLC="$(abspath $(AMLC))" HAIKU_AML_LIB="$(abspath $(AML_LIB))" bash tests/run_text.sh

test-lexical:
	@HAIKU_AML="$(abspath $(AML))" HAIKU_AMLC="$(abspath $(AMLC))" HAIKU_AML_LIB="$(abspath $(AML_LIB))" bash tests/run_lexical.sh

test-cloud:
	@HAIKU_AML="$(abspath $(AML))" HAIKU_AMLC="$(abspath $(AMLC))" HAIKU_AML_LIB="$(abspath $(AML_LIB))" HAIKU_AML_INCLUDE="$(abspath $(AML_INCLUDE))" bash tests/run_cloud.sh

test-form:
	@HAIKU_AML="$(abspath $(AML))" HAIKU_AMLC="$(abspath $(AMLC))" HAIKU_AML_LIB="$(abspath $(AML_LIB))" bash tests/run_form.sh

test-generator:
	@HAIKU_AML="$(abspath $(AML))" HAIKU_AMLC="$(abspath $(AMLC))" HAIKU_AML_LIB="$(abspath $(AML_LIB))" HAIKU_AML_INCLUDE="$(abspath $(AML_INCLUDE))" HAIKU_AML_BRIDGE_LIB="$(abspath $(AML_BRIDGE_LIB))" HAIKU_NOTORCH_LIB="$(abspath $(NOTORCH_LIB))" bash tests/run_generator.sh

test-mathbrain:
	@HAIKU_AML="$(abspath $(AML))" HAIKU_AMLC="$(abspath $(AMLC))" HAIKU_AML_LIB="$(abspath $(AML_LIB))" HAIKU_AML_INCLUDE="$(abspath $(AML_INCLUDE))" HAIKU_AML_BRIDGE_LIB="$(abspath $(AML_BRIDGE_LIB))" HAIKU_NOTORCH_LIB="$(abspath $(NOTORCH_LIB))" bash tests/run_mathbrain.sh

test-rae:
	@HAIKU_AML="$(abspath $(AML))" HAIKU_AMLC="$(abspath $(AMLC))" HAIKU_AML_LIB="$(abspath $(AML_LIB))" HAIKU_AML_INCLUDE="$(abspath $(AML_INCLUDE))" HAIKU_AML_BRIDGE_LIB="$(abspath $(AML_BRIDGE_LIB))" HAIKU_NOTORCH_LIB="$(abspath $(NOTORCH_LIB))" bash tests/run_rae.sh

test-tokenizer:
	@HAIKU_AML="$(abspath $(AML))" HAIKU_AMLC="$(abspath $(AMLC))" HAIKU_AML_LIB="$(abspath $(AML_LIB))" HAIKU_AML_INCLUDE="$(abspath $(AML_INCLUDE))" HAIKU_AML_BRIDGE_LIB="$(abspath $(AML_BRIDGE_LIB))" HAIKU_NOTORCH_LIB="$(abspath $(NOTORCH_LIB))" bash tests/run_tokenizer.sh

test-foreground:
	@HAIKU_AML="$(abspath $(AML))" HAIKU_AMLC="$(abspath $(AMLC))" HAIKU_AML_LIB="$(abspath $(AML_LIB))" HAIKU_AML_INCLUDE="$(abspath $(AML_INCLUDE))" HAIKU_AML_BRIDGE_LIB="$(abspath $(AML_BRIDGE_LIB))" HAIKU_NOTORCH_LIB="$(abspath $(NOTORCH_LIB))" bash tests/run_foreground.sh

test-tokenizer-setup:
	@bash tests/tokenizer_setup.sh

test-state:
	@HAIKU_AML="$(abspath $(AML))" HAIKU_AMLC="$(abspath $(AMLC))" HAIKU_AML_LIB="$(abspath $(AML_LIB))" HAIKU_AML_INCLUDE="$(abspath $(AML_INCLUDE))" HAIKU_AML_BRIDGE_LIB="$(abspath $(AML_BRIDGE_LIB))" HAIKU_NOTORCH_LIB="$(abspath $(NOTORCH_LIB))" bash tests/run_state.sh
