DOXYGEN_CONFIG=doxygen.cfg
DOXYGEN_CONFIG_PRE=doxygen.cfg.in

.PHONY: all clean

$(DOXYGEN_CONFIG):
	cp ./$(DOXYGEN_CONFIG_PRE) ./$(DOXYGEN_CONFIG);
	doxygen $(DOXYGEN_CONFIG)

all: $(DOXYGEN_CONFIG)
	echo DOCUMENTATION\ GENERATION

clean:
	rm -f ./$(DOXYGEN_CONFIG);
	cd doxygen; rm -rf html

