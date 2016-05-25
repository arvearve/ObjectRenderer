all: convert scale clean

# Download tools to manipulate PLY files
tmp/plytools.tar.gz:
	@mkdir -p $(@D)
	@echo "Downloading PLY Tools"
	@wget -O $@ http://www.cc.gatech.edu/projects/large_models/files/ply.tar.gz -q --show-progress

bin/boundply: tmp/plytools.tar.gz src/boundply.c.patch
	@mkdir -p $(@D)
	@mkdir -p tmp/plytools
	@tar -xzf tmp/plytools.tar.gz -C tmp/plytools --strip-components 1 ply/makefile ply/boundply.c ply/ply.h ply/ply.c
	@patch tmp/plytools/boundply.c src/boundply.c.patch
	@$(MAKE) -C tmp/plytools boundply
	@mv tmp/plytools/boundply $@

bin/xformply: tmp/plytools.tar.gz src/xformply.c.patch
	@mkdir -p $(@D)
	@mkdir -p tmp/plytools
	@tar -xzf tmp/plytools.tar.gz -C tmp/plytools --strip-components 1 ply/makefile ply/xformply.c ply/ply.h ply/ply.c
	@patch tmp/plytools/xformply.c src/xformply.c.patch
	@$(MAKE) -C tmp/plytools xformply
	@mv tmp/plytools/xformply $@

bin/normalsply: tmp/plytools.tar.gz src/normalsply.c.patch
	@mkdir -p $(@D)
	@mkdir -p tmp/plytools
	@tar -xzf tmp/plytools.tar.gz -C tmp/plytools --strip-components 1 ply/makefile ply/normalsply.c ply/ply.h ply/ply.c
	@patch tmp/plytools/normalsply.c src/normalsply.c.patch
	@$(MAKE) -C tmp/plytools normalsply
	@mv tmp/plytools/normalsply $@

# Download the Princeton Shape Benchmark
tmp/benchmark.zip:
	@mkdir -p $(@D)
	@echo "Downloading the Princeton Shape Benchmark"
	@wget -O $@ http://shape.cs.princeton.edu/benchmark/download/psb_v1.zip -q --show-progress

# Extract models from the benchmark
models/off: tmp/benchmark.zip
	@mkdir -p $@
	@unzip -j $< *.off -d $@

# Convert models from .off format to .ply, translate and scale to known bounding box
# TODO: Create the list of file from zip index.
sources = $(wildcard models/off/*.off)
targets = $(addprefix models/ply/,$(addsuffix .ply,$(basename $(notdir $(sources)))))
models/ply/m%.ply: models/off/m%.off bin/off2ply bin/boundply bin/xformply bin/normalsply
	@mkdir -p $(@D)
	bin/convert < $< > $@

	
convert: $(targets) models/off
	@echo "Converting and scaling models. This may take a while..."

# Build objectrenderer
objectrenderer:
	@ mkdir -p .build
	@ (cd .build && cmake .. && make)


# Build scene for rendering.
# Make renderer work!

clean:
	-rm -rf .build
	-rm objectrenderer

