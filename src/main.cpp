#include "../include/AST.h"
#include "../include/IR.h"
#include "../include/Lexer.h"

extern int yyparse();
extern void initializeLexer();
extern NBlock *programBlock;

//Path to .fuse file
const char *_global_file_name;
const char *_binary_name;

//TODO: flag that outputs not just the result of the program but also the IR generated llvm code.
static bool _verbose_mode = true;


int main(int argc, char **argv) {

    if (argc != 3) {
        std::cerr << "No input file or binary name provided" << std::endl;
        exit(1);
    } else {

        //verify that only .fuse files are accepted
        auto has_extension = [&](const std::string &filename, const std::string &extension) {
            if (filename.length() >= extension.length()) {
                //Pass lenghts & content
                return (0 == filename.compare(filename.length() - extension.length(), extension.length(), extension));
            }

            return false;
        };


        if (!has_extension(argv[1], ".fuse")) {

            std::cerr << "Invalid file extension in file name " << argv[1] << std::endl;
            exit(1);
        }
        _global_file_name = argv[1];
        assert(argv[1] && "Missing input file in arguments");
        _binary_name = argv[2];
        assert(argv[2] && "Missing binary name in arguments");
    }


    if (yyparse() == 0) {
        try {

            CodeGenContext context(_verbose_mode, _global_file_name, _binary_name);
            context.setTargets();
            context.emitIR(*programBlock);
            context.dumpIR();
            if(_verbose_mode)
              context.runCode();


        } catch (const std::runtime_error &e) {
            std::cerr << e.what() << std::flush;
        }

    } else {

        std::cerr << "Parsing failed" << std::endl;
    }

    return 0;
}
