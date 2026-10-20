AML_ROOT ?= ../ariannamethod.ai
AML ?= $(AML_ROOT)/runner/aml-notorch
AMLC ?= $(AML_ROOT)/tools/amlc
AML_LIB ?= $(AML_ROOT)/libaml.a
AML_INCLUDE ?= $(AML_ROOT)/core
AML_BRIDGE_LIB ?= $(AML_ROOT)/libaml_notorch.a
NOTORCH_ROOT ?= ../notorch
NOTORCH_LIB ?= $(NOTORCH_ROOT)/libnotorch.a

.PHONY: test test-numerical test-text test-lexical test-cloud test-form test-generator test-mathbrain test-rae test-tokenizer test-tokenizer-setup test-foreground test-state
.PHONY: test-background-pending test-background-cli test-iceberg-background

test: test-background-pending test-background-cli test-iceberg-background

test-background-cli:
	@HAIKU_AML="$(abspath $(AML))" HAIKU_AMLC="$(abspath $(AMLC))" HAIKU_AML_LIB="$(abspath $(AML_LIB))" HAIKU_AML_BRIDGE_LIB="$(abspath $(AML_BRIDGE_LIB))" HAIKU_NOTORCH_LIB="$(abspath $(NOTORCH_LIB))" python3 tests/background_cli.py

test-background-pending test-iceberg-background:
	@HAIKU_AML="$(abspath $(AML))" HAIKU_AMLC="$(abspath $(AMLC))" HAIKU_AML_LIB="$(abspath $(AML_LIB))" HAIKU_AML_BRIDGE_LIB="$(abspath $(AML_BRIDGE_LIB))" HAIKU_NOTORCH_LIB="$(abspath $(NOTORCH_LIB))" bash tests/run_$(subst -,_,$(@:test-%=%)).sh
.PHONY: test-inner test-metahaiku test-overthinkg test-inner-state test-inner-cli
.PHONY: test-dream-life test-phase4 test-dream test-journal test-dream-state test-dream-cli
.PHONY: test-owned-tail test-parliament test-lexical-sources test-parliament-contact test-iceberg test-janus-balance test-iceberg-janus
.PHONY: test-contact-learning test-iceberg-learning test-contact-symbols test-iceberg-symbols
.PHONY: test-contact-exchange test-iceberg-exchange
.PHONY: test-contact-concept-numerical test-contact-concept-experience test-iceberg-concepts

test: test-living-form test-living-features test-living-history test-living-pressure test-living-session test-contact-neural test-iceberg-neural test-numerical test-text test-lexical test-cloud test-form test-generator test-mathbrain test-rae test-tokenizer test-foreground test-state test-inner test-dream-life test-owned-tail test-parliament test-lexical-sources test-parliament-contact test-iceberg test-janus-balance test-iceberg-janus test-contact-learning test-iceberg-learning test-contact-symbols test-iceberg-symbols test-contact-exchange test-iceberg-exchange test-contact-concept-numerical test-contact-concept-experience test-iceberg-concepts

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

test-inner: test-metahaiku test-overthinkg test-inner-state test-inner-cli

test-metahaiku:
	@HAIKU_AML="$(abspath $(AML))" HAIKU_AMLC="$(abspath $(AMLC))" HAIKU_AML_LIB="$(abspath $(AML_LIB))" HAIKU_AML_INCLUDE="$(abspath $(AML_INCLUDE))" HAIKU_AML_BRIDGE_LIB="$(abspath $(AML_BRIDGE_LIB))" HAIKU_NOTORCH_LIB="$(abspath $(NOTORCH_LIB))" bash tests/run_metahaiku.sh

test-overthinkg:
	@HAIKU_AML="$(abspath $(AML))" HAIKU_AMLC="$(abspath $(AMLC))" HAIKU_AML_LIB="$(abspath $(AML_LIB))" HAIKU_AML_INCLUDE="$(abspath $(AML_INCLUDE))" HAIKU_AML_BRIDGE_LIB="$(abspath $(AML_BRIDGE_LIB))" HAIKU_NOTORCH_LIB="$(abspath $(NOTORCH_LIB))" bash tests/run_overthinkg.sh

test-inner-state:
	@HAIKU_AML="$(abspath $(AML))" HAIKU_AMLC="$(abspath $(AMLC))" HAIKU_AML_LIB="$(abspath $(AML_LIB))" HAIKU_AML_INCLUDE="$(abspath $(AML_INCLUDE))" HAIKU_AML_BRIDGE_LIB="$(abspath $(AML_BRIDGE_LIB))" HAIKU_NOTORCH_LIB="$(abspath $(NOTORCH_LIB))" bash tests/run_inner_state.sh

test-inner-cli:
	@HAIKU_AML="$(abspath $(AML))" HAIKU_AMLC="$(abspath $(AMLC))" HAIKU_AML_LIB="$(abspath $(AML_LIB))" HAIKU_AML_INCLUDE="$(abspath $(AML_INCLUDE))" HAIKU_AML_BRIDGE_LIB="$(abspath $(AML_BRIDGE_LIB))" HAIKU_NOTORCH_LIB="$(abspath $(NOTORCH_LIB))" bash tests/run_inner_cli.sh

test-dream-life: test-phase4 test-dream test-journal test-dream-state test-dream-cli

test-phase4 test-dream test-journal test-dream-state test-dream-cli test-owned-tail test-parliament test-lexical-sources test-parliament-contact test-iceberg test-janus-balance test-iceberg-janus test-contact-learning test-iceberg-learning test-contact-symbols test-iceberg-symbols test-contact-exchange test-iceberg-exchange test-contact-concept-numerical test-contact-concept-experience test-iceberg-concepts:
	@HAIKU_AML="$(abspath $(AML))" HAIKU_AMLC="$(abspath $(AMLC))" HAIKU_AML_LIB="$(abspath $(AML_LIB))" HAIKU_AML_INCLUDE="$(abspath $(AML_INCLUDE))" HAIKU_AML_BRIDGE_LIB="$(abspath $(AML_BRIDGE_LIB))" HAIKU_NOTORCH_LIB="$(abspath $(NOTORCH_LIB))" bash tests/run_$(subst -,_,$(@:test-%=%)).sh

.PHONY: test-living-form test-living-features test-living-history test-living-pressure test-living-session test-contact-neural test-iceberg-neural
test-living-form test-living-features test-living-history test-living-pressure test-living-session test-contact-neural test-iceberg-neural:
	@HAIKU_AML="$(abspath $(AML))" HAIKU_AMLC="$(abspath $(AMLC))" HAIKU_AML_LIB="$(abspath $(AML_LIB))" HAIKU_AML_INCLUDE="$(abspath $(AML_INCLUDE))" HAIKU_AML_BRIDGE_LIB="$(abspath $(AML_BRIDGE_LIB))" HAIKU_NOTORCH_LIB="$(abspath $(NOTORCH_LIB))" bash tests/run_$(subst -,_,$(@:test-%=%)).sh
