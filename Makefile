AML_ROOT ?= ../ariannamethod.ai
AML ?= $(AML_ROOT)/runner/aml
AMLC ?= $(AML_ROOT)/tools/amlc
AML_LIB ?= $(AML_ROOT)/libaml.a

.PHONY: test

test:
	@HAIKU_AML="$(abspath $(AML))" HAIKU_AMLC="$(abspath $(AMLC))" HAIKU_AML_LIB="$(abspath $(AML_LIB))" bash tests/run_numerical.sh
