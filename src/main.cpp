#include "AST.h"
#include "IR.h"
#include "Lexer.h"
#include <CLI11.hpp>//CLI parser
#include <FuseHandler.h>
#include <algorithm>
#include <iomanip>

extern int yyparse();
//extern void initializeLexer();
extern NBlock *programBlock;
extern FuseHandler fusehandler;

//Path to .fuse file
std::string _global_file_path;
std::string _binary_name;


//TODO: flag that outputs not just the result of the program but also the IR generated llvm code.
static bool _verbose_mode = false;
static bool wasm_compilation = false;

int main(int argc, char **argv) {

    //TODO: Figure out this thing
    CLI::App app{"Fuse experimental Compiler"};
    argv = app.ensure_utf8(argv);

    app.add_option("source", _global_file_path, "Path to .fuse file")
            ->required()
            ->check(CLI::ExistingFile)
            ->check([](const std::string &filename) {// -> Custom validator, passed as lambda/functor
                const std::string &extension = ".fuse";
                if (filename.length() >= extension.length()) {
                    //Pass lenghts & content
                    if (0 == filename.compare(filename.length() - extension.length(), extension.length(), extension)) {
                        return std::string();// -> Success
                    } else {

                        return std::string("File must have .fuse extension");
                    }
                }

                return std::string("File must have .fuse extension");
            });


    app.add_flag("-v,--verbose", _verbose_mode, "Toggle Verbose mode");

    //Wasm flag
    app.add_flag("-w, --wasm", wasm_compilation, "Generate wasm files");

    app.add_option("binary", _binary_name, "Generated binary name")
            ->required();


    CLI11_PARSE(app, argc, argv);

    std::cout << "Compiling " << _global_file_path << std::endl;

    if (yyparse() == 0) {
        try {

            CodeGenContext context(_verbose_mode, _global_file_path, _binary_name, wasm_compilation);
            context.setTargets();
            context.emitIR(*programBlock);
            context.dumpIR();

            if (_verbose_mode) {
                std::cout << std::endl;
                std::cout << std::setw(40) << std::string("OUTPUT") << std::endl;
                std::cout << std::string(80, '=') << std::endl;
                context.runCode();
                std::cout << std::string(80, '=') << std::endl;
            }


        } catch (const std::runtime_error &e) {
            std::cerr << e.what() << std::flush;
        }

    } else {

        std::cerr << "Parsing failed" << std::endl;
    }

    return 0;
}
