FLAGS = -ggdb3 -std=c++17 -O0 \
	-Wall -Wextra -Weffc++ -Waggressive-loop-optimizations -Wc++14-compat \
	-Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts \
	-Wconditionally-supported -Wconversion -Wctor-dtor-privacy \
	-Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security \
	-Wformat-signedness -Wformat=2 -Winline -Wlogical-op \
	-Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked \
	-Wpointer-arith -Winit-self -Wredundant-decls -Wshadow \
	-Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=2 \
	-Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types \
	-Wsuggest-override -Wswitch-default -Wswitch-enum -Wundef \
	-Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros \
	-Wno-literal-suffix -Wno-missing-field-initializers \
	-Wno-narrowing -Wno-old-style-cast -Wno-varargs \
    -fcheck-new -fsized-deallocation -fstack-protector \
	-fstrict-overflow -flto-odr-type-merging -fno-omit-frame-pointer \
	-Wlarger-than=32768 -Wstack-usage=8192 -pie -fPIE -Werror=vla \
	-fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr

BUILDDIR = build
OBJDIR = $(BUILDDIR)/obj
SRCDIR = source


FILES = $(OBJDIR)/diff/main.o $(OBJDIR)/diff/diff_var_table.o $(OBJDIR)/tree/tree_io.o \
	$(OBJDIR)/tree/tree.o $(OBJDIR)/diff/diff.o $(OBJDIR)/diff/diff_process.o \
	$(OBJDIR)/diff/diff_evaluate.o $(OBJDIR)/diff/diff_optimize.o $(OBJDIR)/tree/tree_parse.o
DEBUG_FILES = $(OBJDIR)/graph_dump/graph_generator.o $(OBJDIR)/graph_dump/html_builder.o \
	$(OBJDIR)/tex_dump/tex.o
FLAGS += -Iinclude

OUTPUT_NAME = diffuzor


clean: 
	@echo "cleaning up object files..."
	@rm -rf $(OBJDIR)
	@rm $(BUILDDIR)/$(OUTPUT_NAME)


run:
	@cd $(BUILDDIR) && ./$(OUTPUT_NAME)


debug: FLAGS += -DDEBUG
debug: $(FILES) $(DEBUG_FILES)
	@g++ $(FILES) $(DEBUG_FILES) $(FLAGS) -o $(BUILDDIR)/$(OUTPUT_NAME)


release: $(FILES)
	@g++ $(FILES) $(FLAGS) -o $(BUILDDIR)/$(OUTPUT_NAME)


$(OBJDIR)/diff/%.o: $(SRCDIR)/diff/%.cpp
	@mkdir -p $(OBJDIR)/diff
	@g++ -c $< $(FLAGS) -o $@


$(OBJDIR)/tree/%.o: $(SRCDIR)/tree/%.cpp
	@mkdir -p $(OBJDIR)/tree
	@g++ -c $< $(FLAGS) -o $@


$(OBJDIR)/graph_dump/%.o: $(SRCDIR)/graph_dump/%.cpp
	@mkdir -p $(OBJDIR)/graph_dump
	@g++ -c $< $(FLAGS) -o $@

$(OBJDIR)/tex_dump/%.o: $(SRCDIR)/tex_dump/%.cpp
	@mkdir -p $(OBJDIR)/tex_dump
	@g++ -c $< $(FLAGS) -o $@
