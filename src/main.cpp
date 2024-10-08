#include "AST.h"
#include "IR.h"
#include "Lexer.h"
#include "parser.hpp"
#include <CLI11.hpp>//CLI parser
#include <FuseHandler.h>
#include <iomanip>


// extern int yyparse();
//extern void initializeLexer();
extern std::unique_ptr<NBlock> programBlock;
extern bool _verbose_mode;
extern FuseHandler fusehandler;
extern Lexer *lexerInstance;

//Path to .fuse file
std::string _global_file_path;
std::string _binary_name;

using namespace yy;


//TODO: flag that outputs not just the result of the program but also the IR generated llvm code.

static bool binary_compilation = false;
//Creates a .ll file
static bool dump_mode = false;


int main(int argc, char **argv) {
    const char *var = argv[0];
    std::cout << var << "\n";

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


    //Includes output of the lexer process plus Lexer buildup
    app.add_flag("-v,--verbose", _verbose_mode, "Toggle Verbose mode");
    app.add_flag("-o,--binary", binary_compilation, "Compile to binary file");
    app.add_flag("-d,--dump", dump_mode, "Dump IR to file");

    CLI::Option *opt = app.add_option("bn-name", _binary_name, "Generated binary name");
    // opt->needs("-o"); Could use this but doesn't provide a good error message

    app.parse(argc, argv);

    if (binary_compilation && *opt) {
        if (!opt->count()) {
            std::cerr << "INTERFUSE: -o flag requires the binary name to compile.\n";
            exit(0);
        }
    }

    //dump flag
    CLI::Option *d_opt = app.add_option("dumo-filename", _binary_name, "Dump file name");

    if (dump_mode && *d_opt) {
        if (!d_opt->count()) {
            std::cerr << "INTERFUSE: -d flag requires the dump file name.\n";
            exit(0);
        }
    }

    CLI11_PARSE(app, argc, argv);

    try {

        yy::fuse_parser parser;
        if (parser.parse() == 0) {


            CodeGenContext context(_verbose_mode, binary_compilation, _global_file_path,
                                   _binary_name, dump_mode);
            context.emitIR(*programBlock);


            if (context.binary_compilation || context.dump_mode) {
                context.dumpIR();
            }


            if (_verbose_mode) {
                std::cout << std::endl;
                std::cout << std::setw(40) << std::string("OUTPUT") << std::endl;
                std::cout << std::string(80, '=') << std::endl;
                context.runCode();
                std::cout << std::string(80, '=') << std::endl;
            } else {
                context.runCode();
            }
        }


    } catch (const std::runtime_error &e) {
        std::cerr << e.what() << std::flush;
    }


    //cleanup
    delete lexerInstance;


    return 0;
}
