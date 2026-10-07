AML_ROOT ?= ../ariannamethod.ai
AML ?= $(AML_ROOT)/runner/aml
AMLC ?= $(AML_ROOT)/tools/amlc
AML_LIB ?= $(AML_ROOT)/libaml.a
AML_INCLUDE ?= $(AML_ROOT)/core

.PHONY: test test-numerical test-text test-lexical test-cloud

test: test-numerical test-text test-lexical test-cloud

test-numerical:
	@HAIKU_AML="$(abspath $(AML))" HAIKU_AMLC="$(abspath $(AMLC))" HAIKU_AML_LIB="$(abspath $(AML_LIB))" bash tests/run_numerical.sh

test-text:
	@HAIKU_AML="$(abspath $(AML))" HAIKU_AMLC="$(abspath $(AMLC))" HAIKU_AML_LIB="$(abspath $(AML_LIB))" bash tests/run_text.sh

test-lexical:
	@HAIKU_AML="$(abspath $(AML))" HAIKU_AMLC="$(abspath $(AMLC))" HAIKU_AML_LIB="$(abspath $(AML_LIB))" bash tests/run_lexical.sh

test-cloud:
	@HAIKU_AML="$(abspath $(AML))" HAIKU_AMLC="$(abspath $(AMLC))" HAIKU_AML_LIB="$(abspath $(AML_LIB))" HAIKU_AML_INCLUDE="$(abspath $(AML_INCLUDE))" bash tests/run_cloud.sh
