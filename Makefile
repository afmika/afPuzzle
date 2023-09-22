# GNU Make workspace makefile autogenerated by Premake

ifndef config
  config=debug
endif

ifndef verbose
  SILENT = @
endif

ifeq ($(config),debug)
  afPuzzle_config = debug
endif
ifeq ($(config),release)
  afPuzzle_config = release
endif

PROJECTS := afPuzzle

.PHONY: all clean help $(PROJECTS) 

all: $(PROJECTS)

afPuzzle:
ifneq (,$(afPuzzle_config))
	@echo "==== Building afPuzzle ($(afPuzzle_config)) ===="
	@${MAKE} --no-print-directory -C . -f afPuzzle.make config=$(afPuzzle_config)
endif

clean:
	@${MAKE} --no-print-directory -C . -f afPuzzle.make clean

help:
	@echo "Usage: make [config=name] [target]"
	@echo ""
	@echo "CONFIGURATIONS:"
	@echo "  debug"
	@echo "  release"
	@echo ""
	@echo "TARGETS:"
	@echo "   all (default)"
	@echo "   clean"
	@echo "   afPuzzle"
	@echo ""
	@echo "For more information, see https://github.com/premake/premake-core/wiki"