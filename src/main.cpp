#include "AST.h"
#include "IR.h"
#include "Lexer.h"
#include "parser.hpp"
#include <CLI11.hpp>//CLI parser
#include <FuseHandler.h>
#include <httplib.h>
#include <iomanip>

#define CPPHTTPLIB_OPENSSL_SUPPORT

// extern int yyparse();
//extern void initializeLexer();
extern std::unique_ptr<NBlock> programBlock;
extern FuseHandler fusehandler;

//Path to .fuse file
std::string _global_file_path;
std::string _binary_name;

using namespace yy;


//TODO: flag that outputs not just the result of the program but also the IR generated llvm code.
static bool _verbose_mode = false;
static bool wasm_compilation = false;


int main(int argc, char **argv) {
    //
    // try {
    //
    //     yy::fuse_parser parser;
    //     if (parser.parse() == 0) {
    //
    //
    //         CodeGenContext context(_verbose_mode, _global_file_path, _binary_name, wasm_compilation);
    //         context.setTargets();
    //         context.emitIR(*programBlock);
    //         context.dumpIR();
    //
    //         if (_verbose_mode) {
    //             std::cout << std::endl;
    //             std::cout << std::setw(40) << std::string("OUTPUT") << std::endl;
    //             std::cout << std::string(80, '=') << std::endl;
    //             context.runCode();
    //             std::cout << std::string(80, '=') << std::endl;
    //         }
    //     }
    //
    //
    // } catch (const std::runtime_error &e) {
    //     std::cerr << e.what() << std::flush;
    // }


    // HTTP
    httplib::Server svr;


    svr.Get("/", [](const httplib::Request &, httplib::Response &res) {
        res.set_content("Hello World!", "text/plain");
    });


    svr.Get("/hi", [](const httplib::Request &, httplib::Response &res) {
        res.set_content("Hello Rolex", "text/plain");
    });


    svr.listen("0.0.0.0", 8080);


    return 0;
}
