// A Bison parser, made by GNU Bison 3.8.2.

// Skeleton interface for Bison LALR(1) parsers in C++

// Copyright (C) 2002-2015, 2018-2021 Free Software Foundation, Inc.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

// As a special exception, you may create a larger work that contains
// part or all of the Bison parser skeleton and distribute that work
// under terms of your choice, so long as that work isn't itself a
// parser generator using the skeleton or a modified version thereof
// as a parser skeleton.  Alternatively, if you modify or redistribute
// the parser skeleton itself, you may (at your option) remove this
// special exception, which will cause the skeleton and the resulting
// Bison output files to be licensed under the GNU General Public
// License without this special exception.

// This special exception was added by the Free Software Foundation in
// version 2.2 of Bison.


/**
 ** \file /Users/davidfeliperiveraguerra/dev/Interfuse-Project/compiler/include/parser.hpp
 ** Define the yy::parser class.
 */

// C++ LALR(1) parser skeleton written by Akim Demaille.

// DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
// especially those whose name start with YY_ or yy_.  They are
// private implementation details that can be changed or removed.

#ifndef YY_YY_USERS_DAVIDFELIPERIVERAGUERRA_DEV_INTERFUSE_PROJECT_COMPILER_INCLUDE_PARSER_HPP_INCLUDED
# define YY_YY_USERS_DAVIDFELIPERIVERAGUERRA_DEV_INTERFUSE_PROJECT_COMPILER_INCLUDE_PARSER_HPP_INCLUDED
// "%code requires" blocks.
#line 11 "/Users/davidfeliperiveraguerra/dev/Interfuse-Project/compiler/src/parser.y"


    #include "Lexer.h"
    #include "AST.h"
    //#include "FuseHandler.h"
    #include <iostream>
    #include <vector>
    #include <memory>
    #include <variant>

    using namespace std;

#line 62 "/Users/davidfeliperiveraguerra/dev/Interfuse-Project/compiler/include/parser.hpp"


# include <cstdlib> // std::abort
# include <iostream>
# include <stdexcept>
# include <string>
# include <vector>

#if defined __cplusplus
# define YY_CPLUSPLUS __cplusplus
#else
# define YY_CPLUSPLUS 199711L
#endif

// Support move semantics when possible.
#if 201103L <= YY_CPLUSPLUS
# define YY_MOVE           std::move
# define YY_MOVE_OR_COPY   move
# define YY_MOVE_REF(Type) Type&&
# define YY_RVREF(Type)    Type&&
# define YY_COPY(Type)     Type
#else
# define YY_MOVE
# define YY_MOVE_OR_COPY   copy
# define YY_MOVE_REF(Type) Type&
# define YY_RVREF(Type)    const Type&
# define YY_COPY(Type)     const Type&
#endif

// Support noexcept when possible.
#if 201103L <= YY_CPLUSPLUS
# define YY_NOEXCEPT noexcept
# define YY_NOTHROW
#else
# define YY_NOEXCEPT
# define YY_NOTHROW throw ()
#endif

// Support constexpr when possible.
#if 201703 <= YY_CPLUSPLUS
# define YY_CONSTEXPR constexpr
#else
# define YY_CONSTEXPR
#endif



#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif

namespace yy {
#line 197 "/Users/davidfeliperiveraguerra/dev/Interfuse-Project/compiler/include/parser.hpp"




  /// A Bison parser.
  class fuse_parser
  {
  public:
#ifdef YYSTYPE
# ifdef __GNUC__
#  pragma GCC message "bison: do not #define YYSTYPE in C++, use %define api.value.type"
# endif
    typedef YYSTYPE value_type;
#else
  /// A buffer to store and retrieve objects.
  ///
  /// Sort of a variant, but does not keep track of the nature
  /// of the stored data, since that knowledge is available
  /// via the current parser state.
  class value_type
  {
  public:
    /// Type of *this.
    typedef value_type self_type;

    /// Empty construction.
    value_type () YY_NOEXCEPT
      : yyraw_ ()
    {}

    /// Construct and fill.
    template <typename T>
    value_type (YY_RVREF (T) t)
    {
      new (yyas_<T> ()) T (YY_MOVE (t));
    }

#if 201103L <= YY_CPLUSPLUS
    /// Non copyable.
    value_type (const self_type&) = delete;
    /// Non copyable.
    self_type& operator= (const self_type&) = delete;
#endif

    /// Destruction, allowed only if empty.
    ~value_type () YY_NOEXCEPT
    {}

# if 201103L <= YY_CPLUSPLUS
    /// Instantiate a \a T in here from \a t.
    template <typename T, typename... U>
    T&
    emplace (U&&... u)
    {
      return *new (yyas_<T> ()) T (std::forward <U>(u)...);
    }
# else
    /// Instantiate an empty \a T in here.
    template <typename T>
    T&
    emplace ()
    {
      return *new (yyas_<T> ()) T ();
    }

    /// Instantiate a \a T in here from \a t.
    template <typename T>
    T&
    emplace (const T& t)
    {
      return *new (yyas_<T> ()) T (t);
    }
# endif

    /// Instantiate an empty \a T in here.
    /// Obsolete, use emplace.
    template <typename T>
    T&
    build ()
    {
      return emplace<T> ();
    }

    /// Instantiate a \a T in here from \a t.
    /// Obsolete, use emplace.
    template <typename T>
    T&
    build (const T& t)
    {
      return emplace<T> (t);
    }

    /// Accessor to a built \a T.
    template <typename T>
    T&
    as () YY_NOEXCEPT
    {
      return *yyas_<T> ();
    }

    /// Const accessor to a built \a T (for %printer).
    template <typename T>
    const T&
    as () const YY_NOEXCEPT
    {
      return *yyas_<T> ();
    }

    /// Swap the content with \a that, of same type.
    ///
    /// Both variants must be built beforehand, because swapping the actual
    /// data requires reading it (with as()), and this is not possible on
    /// unconstructed variants: it would require some dynamic testing, which
    /// should not be the variant's responsibility.
    /// Swapping between built and (possibly) non-built is done with
    /// self_type::move ().
    template <typename T>
    void
    swap (self_type& that) YY_NOEXCEPT
    {
      std::swap (as<T> (), that.as<T> ());
    }

    /// Move the content of \a that to this.
    ///
    /// Destroys \a that.
    template <typename T>
    void
    move (self_type& that)
    {
# if 201103L <= YY_CPLUSPLUS
      emplace<T> (std::move (that.as<T> ()));
# else
      emplace<T> ();
      swap<T> (that);
# endif
      that.destroy<T> ();
    }

# if 201103L <= YY_CPLUSPLUS
    /// Move the content of \a that to this.
    template <typename T>
    void
    move (self_type&& that)
    {
      emplace<T> (std::move (that.as<T> ()));
      that.destroy<T> ();
    }
#endif

    /// Copy the content of \a that to this.
    template <typename T>
    void
    copy (const self_type& that)
    {
      emplace<T> (that.as<T> ());
    }

    /// Destroy the stored \a T.
    template <typename T>
    void
    destroy ()
    {
      as<T> ().~T ();
    }

  private:
#if YY_CPLUSPLUS < 201103L
    /// Non copyable.
    value_type (const self_type&);
    /// Non copyable.
    self_type& operator= (const self_type&);
#endif

    /// Accessor to raw memory as \a T.
    template <typename T>
    T*
    yyas_ () YY_NOEXCEPT
    {
      void *yyp = yyraw_;
      return static_cast<T*> (yyp);
     }

    /// Const accessor to raw memory as \a T.
    template <typename T>
    const T*
    yyas_ () const YY_NOEXCEPT
    {
      const void *yyp = yyraw_;
      return static_cast<const T*> (yyp);
     }

    /// An auxiliary type to compute the largest semantic type.
    union union_type
    {
      // fn_call_args
      char dummy1[sizeof (shared_ptr<vector<shared_ptr<NExpression>>>)];

      // program
      // stmts
      // block
      char dummy2[sizeof (std::shared_ptr<NBlock>)];

      // else_stmt
      char dummy3[sizeof (std::shared_ptr<NElseStatement>)];

      // expr
      // numeric
      // string
      char dummy4[sizeof (std::shared_ptr<NExpression>)];

      // id
      char dummy5[sizeof (std::shared_ptr<NIdentifier>)];

      // stmt
      // fn_decl
      // for_stmt
      // if_stmt
      // break_stmt
      // continue_stmt
      char dummy6[sizeof (std::shared_ptr<NStatement>)];

      // var_decl
      char dummy7[sizeof (std::shared_ptr<NVariableDeclaration>)];

      // TKNUMBER
      // TKDOUBLE
      // TKSTRING
      // TKPLUS
      // TKMINUS
      // TKMULT
      // TKDIV
      // TKMOD
      // TKRETURN
      // TKCOMMA
      // TKDOT
      // TKARROW
      // TKCOLON
      // TKRANGE_INCLUSIVE
      // TKLINEBREAK
      // TKASSIGNMENT
      // TKLESS
      // TKGREATER
      // TKLESS_EQUAL
      // TKGREATER_EQUAL
      // TKEQUAL
      // TKNOT_EQUAL
      // TKAND
      // TKOR
      // TKNEGATION
      // TKIDENTIFIER
      // TKDATATYPE
      // TKCURLYOPEN
      // TKCURLYCLOSE
      // TKPAROPEN
      // TKPARCLOSE
      // TKFUNCTION_KEY
      // TKIF
      // TKELSE
      // TKELSEIF
      // TKFOR
      // TKIN
      // TKBREAK
      // TKCONT
      // TKINVALID
      // binary_op
      // negation
      // comparison
      char dummy8[sizeof (std::shared_ptr<Token>)];

      // fn_args
      char dummy9[sizeof (std::shared_ptr<VariableList>)];
    };

    /// The size of the largest semantic type.
    enum { size = sizeof (union_type) };

    /// A buffer to store semantic values.
    union
    {
      /// Strongest alignment constraints.
      long double yyalign_me_;
      /// A buffer large enough to store any of the semantic values.
      char yyraw_[size];
    };
  };

#endif
    /// Backward compatibility (Bison 3.8).
    typedef value_type semantic_type;

    /// Symbol locations.
    typedef TokenLocation location_type;

    /// Syntax errors thrown from user actions.
    struct syntax_error : std::runtime_error
    {
      syntax_error (const location_type& l, const std::string& m)
        : std::runtime_error (m)
        , location (l)
      {}

      syntax_error (const syntax_error& s)
        : std::runtime_error (s.what ())
        , location (s.location)
      {}

      ~syntax_error () YY_NOEXCEPT YY_NOTHROW;

      location_type location;
    };

    /// Token kinds.
    struct token
    {
      enum token_kind_type
      {
        YYEMPTY = -2,
    YYEOF = 0,                     // "end of file"
    YYerror = 256,                 // error
    YYUNDEF = 257,                 // "invalid token"
    TKNUMBER = 258,                // TKNUMBER
    TKDOUBLE = 259,                // TKDOUBLE
    TKSTRING = 260,                // TKSTRING
    TKPLUS = 261,                  // TKPLUS
    TKMINUS = 262,                 // TKMINUS
    TKMULT = 263,                  // TKMULT
    TKDIV = 264,                   // TKDIV
    TKMOD = 265,                   // TKMOD
    TKRETURN = 266,                // TKRETURN
    TKCOMMA = 267,                 // TKCOMMA
    TKDOT = 268,                   // TKDOT
    TKARROW = 269,                 // TKARROW
    TKCOLON = 270,                 // TKCOLON
    TKRANGE_INCLUSIVE = 271,       // TKRANGE_INCLUSIVE
    TKLINEBREAK = 272,             // TKLINEBREAK
    TKASSIGNMENT = 273,            // TKASSIGNMENT
    TKLESS = 274,                  // TKLESS
    TKGREATER = 275,               // TKGREATER
    TKLESS_EQUAL = 276,            // TKLESS_EQUAL
    TKGREATER_EQUAL = 277,         // TKGREATER_EQUAL
    TKEQUAL = 278,                 // TKEQUAL
    TKNOT_EQUAL = 279,             // TKNOT_EQUAL
    TKAND = 280,                   // TKAND
    TKOR = 281,                    // TKOR
    TKNEGATION = 282,              // TKNEGATION
    TKIDENTIFIER = 283,            // TKIDENTIFIER
    TKDATATYPE = 284,              // TKDATATYPE
    TKCURLYOPEN = 285,             // TKCURLYOPEN
    TKCURLYCLOSE = 286,            // TKCURLYCLOSE
    TKPAROPEN = 287,               // TKPAROPEN
    TKPARCLOSE = 288,              // TKPARCLOSE
    TKFUNCTION_KEY = 289,          // TKFUNCTION_KEY
    TKIF = 290,                    // TKIF
    TKELSE = 291,                  // TKELSE
    TKELSEIF = 292,                // TKELSEIF
    TKFOR = 293,                   // TKFOR
    TKIN = 294,                    // TKIN
    TKBREAK = 295,                 // TKBREAK
    TKCONT = 296,                  // TKCONT
    TKINVALID = 297                // TKINVALID
      };
      /// Backward compatibility alias (Bison 3.6).
      typedef token_kind_type yytokentype;
    };

    /// Token kind, as returned by yylex.
    typedef token::token_kind_type token_kind_type;

    /// Backward compatibility alias (Bison 3.6).
    typedef token_kind_type token_type;

    /// Symbol kinds.
    struct symbol_kind
    {
      enum symbol_kind_type
      {
        YYNTOKENS = 43, ///< Number of tokens.
        S_YYEMPTY = -2,
        S_YYEOF = 0,                             // "end of file"
        S_YYerror = 1,                           // error
        S_YYUNDEF = 2,                           // "invalid token"
        S_TKNUMBER = 3,                          // TKNUMBER
        S_TKDOUBLE = 4,                          // TKDOUBLE
        S_TKSTRING = 5,                          // TKSTRING
        S_TKPLUS = 6,                            // TKPLUS
        S_TKMINUS = 7,                           // TKMINUS
        S_TKMULT = 8,                            // TKMULT
        S_TKDIV = 9,                             // TKDIV
        S_TKMOD = 10,                            // TKMOD
        S_TKRETURN = 11,                         // TKRETURN
        S_TKCOMMA = 12,                          // TKCOMMA
        S_TKDOT = 13,                            // TKDOT
        S_TKARROW = 14,                          // TKARROW
        S_TKCOLON = 15,                          // TKCOLON
        S_TKRANGE_INCLUSIVE = 16,                // TKRANGE_INCLUSIVE
        S_TKLINEBREAK = 17,                      // TKLINEBREAK
        S_TKASSIGNMENT = 18,                     // TKASSIGNMENT
        S_TKLESS = 19,                           // TKLESS
        S_TKGREATER = 20,                        // TKGREATER
        S_TKLESS_EQUAL = 21,                     // TKLESS_EQUAL
        S_TKGREATER_EQUAL = 22,                  // TKGREATER_EQUAL
        S_TKEQUAL = 23,                          // TKEQUAL
        S_TKNOT_EQUAL = 24,                      // TKNOT_EQUAL
        S_TKAND = 25,                            // TKAND
        S_TKOR = 26,                             // TKOR
        S_TKNEGATION = 27,                       // TKNEGATION
        S_TKIDENTIFIER = 28,                     // TKIDENTIFIER
        S_TKDATATYPE = 29,                       // TKDATATYPE
        S_TKCURLYOPEN = 30,                      // TKCURLYOPEN
        S_TKCURLYCLOSE = 31,                     // TKCURLYCLOSE
        S_TKPAROPEN = 32,                        // TKPAROPEN
        S_TKPARCLOSE = 33,                       // TKPARCLOSE
        S_TKFUNCTION_KEY = 34,                   // TKFUNCTION_KEY
        S_TKIF = 35,                             // TKIF
        S_TKELSE = 36,                           // TKELSE
        S_TKELSEIF = 37,                         // TKELSEIF
        S_TKFOR = 38,                            // TKFOR
        S_TKIN = 39,                             // TKIN
        S_TKBREAK = 40,                          // TKBREAK
        S_TKCONT = 41,                           // TKCONT
        S_TKINVALID = 42,                        // TKINVALID
        S_YYACCEPT = 43,                         // $accept
        S_program = 44,                          // program
        S_stmts = 45,                            // stmts
        S_stmt = 46,                             // stmt
        S_id = 47,                               // id
        S_var_decl = 48,                         // var_decl
        S_fn_decl = 49,                          // fn_decl
        S_fn_args = 50,                          // fn_args
        S_block = 51,                            // block
        S_for_stmt = 52,                         // for_stmt
        S_if_stmt = 53,                          // if_stmt
        S_else_stmt = 54,                        // else_stmt
        S_break_stmt = 55,                       // break_stmt
        S_continue_stmt = 56,                    // continue_stmt
        S_fn_call_args = 57,                     // fn_call_args
        S_expr = 58,                             // expr
        S_numeric = 59,                          // numeric
        S_string = 60,                           // string
        S_binary_op = 61,                        // binary_op
        S_negation = 62,                         // negation
        S_comparison = 63                        // comparison
      };
    };

    /// (Internal) symbol kind.
    typedef symbol_kind::symbol_kind_type symbol_kind_type;

    /// The number of tokens.
    static const symbol_kind_type YYNTOKENS = symbol_kind::YYNTOKENS;

    /// A complete symbol.
    ///
    /// Expects its Base type to provide access to the symbol kind
    /// via kind ().
    ///
    /// Provide access to semantic value and location.
    template <typename Base>
    struct basic_symbol : Base
    {
      /// Alias to Base.
      typedef Base super_type;

      /// Default constructor.
      basic_symbol () YY_NOEXCEPT
        : value ()
        , location ()
      {}

#if 201103L <= YY_CPLUSPLUS
      /// Move constructor.
      basic_symbol (basic_symbol&& that)
        : Base (std::move (that))
        , value ()
        , location (std::move (that.location))
      {
        switch (this->kind ())
    {
      case symbol_kind::S_fn_call_args: // fn_call_args
        value.move< shared_ptr<vector<shared_ptr<NExpression>>> > (std::move (that.value));
        break;

      case symbol_kind::S_program: // program
      case symbol_kind::S_stmts: // stmts
      case symbol_kind::S_block: // block
        value.move< std::shared_ptr<NBlock> > (std::move (that.value));
        break;

      case symbol_kind::S_else_stmt: // else_stmt
        value.move< std::shared_ptr<NElseStatement> > (std::move (that.value));
        break;

      case symbol_kind::S_expr: // expr
      case symbol_kind::S_numeric: // numeric
      case symbol_kind::S_string: // string
        value.move< std::shared_ptr<NExpression> > (std::move (that.value));
        break;

      case symbol_kind::S_id: // id
        value.move< std::shared_ptr<NIdentifier> > (std::move (that.value));
        break;

      case symbol_kind::S_stmt: // stmt
      case symbol_kind::S_fn_decl: // fn_decl
      case symbol_kind::S_for_stmt: // for_stmt
      case symbol_kind::S_if_stmt: // if_stmt
      case symbol_kind::S_break_stmt: // break_stmt
      case symbol_kind::S_continue_stmt: // continue_stmt
        value.move< std::shared_ptr<NStatement> > (std::move (that.value));
        break;

      case symbol_kind::S_var_decl: // var_decl
        value.move< std::shared_ptr<NVariableDeclaration> > (std::move (that.value));
        break;

      case symbol_kind::S_TKNUMBER: // TKNUMBER
      case symbol_kind::S_TKDOUBLE: // TKDOUBLE
      case symbol_kind::S_TKSTRING: // TKSTRING
      case symbol_kind::S_TKPLUS: // TKPLUS
      case symbol_kind::S_TKMINUS: // TKMINUS
      case symbol_kind::S_TKMULT: // TKMULT
      case symbol_kind::S_TKDIV: // TKDIV
      case symbol_kind::S_TKMOD: // TKMOD
      case symbol_kind::S_TKRETURN: // TKRETURN
      case symbol_kind::S_TKCOMMA: // TKCOMMA
      case symbol_kind::S_TKDOT: // TKDOT
      case symbol_kind::S_TKARROW: // TKARROW
      case symbol_kind::S_TKCOLON: // TKCOLON
      case symbol_kind::S_TKRANGE_INCLUSIVE: // TKRANGE_INCLUSIVE
      case symbol_kind::S_TKLINEBREAK: // TKLINEBREAK
      case symbol_kind::S_TKASSIGNMENT: // TKASSIGNMENT
      case symbol_kind::S_TKLESS: // TKLESS
      case symbol_kind::S_TKGREATER: // TKGREATER
      case symbol_kind::S_TKLESS_EQUAL: // TKLESS_EQUAL
      case symbol_kind::S_TKGREATER_EQUAL: // TKGREATER_EQUAL
      case symbol_kind::S_TKEQUAL: // TKEQUAL
      case symbol_kind::S_TKNOT_EQUAL: // TKNOT_EQUAL
      case symbol_kind::S_TKAND: // TKAND
      case symbol_kind::S_TKOR: // TKOR
      case symbol_kind::S_TKNEGATION: // TKNEGATION
      case symbol_kind::S_TKIDENTIFIER: // TKIDENTIFIER
      case symbol_kind::S_TKDATATYPE: // TKDATATYPE
      case symbol_kind::S_TKCURLYOPEN: // TKCURLYOPEN
      case symbol_kind::S_TKCURLYCLOSE: // TKCURLYCLOSE
      case symbol_kind::S_TKPAROPEN: // TKPAROPEN
      case symbol_kind::S_TKPARCLOSE: // TKPARCLOSE
      case symbol_kind::S_TKFUNCTION_KEY: // TKFUNCTION_KEY
      case symbol_kind::S_TKIF: // TKIF
      case symbol_kind::S_TKELSE: // TKELSE
      case symbol_kind::S_TKELSEIF: // TKELSEIF
      case symbol_kind::S_TKFOR: // TKFOR
      case symbol_kind::S_TKIN: // TKIN
      case symbol_kind::S_TKBREAK: // TKBREAK
      case symbol_kind::S_TKCONT: // TKCONT
      case symbol_kind::S_TKINVALID: // TKINVALID
      case symbol_kind::S_binary_op: // binary_op
      case symbol_kind::S_negation: // negation
      case symbol_kind::S_comparison: // comparison
        value.move< std::shared_ptr<Token> > (std::move (that.value));
        break;

      case symbol_kind::S_fn_args: // fn_args
        value.move< std::shared_ptr<VariableList> > (std::move (that.value));
        break;

      default:
        break;
    }

      }
#endif

      /// Copy constructor.
      basic_symbol (const basic_symbol& that);

      /// Constructors for typed symbols.
#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, location_type&& l)
        : Base (t)
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const location_type& l)
        : Base (t)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, shared_ptr<vector<shared_ptr<NExpression>>>&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const shared_ptr<vector<shared_ptr<NExpression>>>& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, std::shared_ptr<NBlock>&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const std::shared_ptr<NBlock>& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, std::shared_ptr<NElseStatement>&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const std::shared_ptr<NElseStatement>& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, std::shared_ptr<NExpression>&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const std::shared_ptr<NExpression>& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, std::shared_ptr<NIdentifier>&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const std::shared_ptr<NIdentifier>& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, std::shared_ptr<NStatement>&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const std::shared_ptr<NStatement>& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, std::shared_ptr<NVariableDeclaration>&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const std::shared_ptr<NVariableDeclaration>& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, std::shared_ptr<Token>&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const std::shared_ptr<Token>& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, std::shared_ptr<VariableList>&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const std::shared_ptr<VariableList>& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

      /// Destroy the symbol.
      ~basic_symbol ()
      {
        clear ();
      }



      /// Destroy contents, and record that is empty.
      void clear () YY_NOEXCEPT
      {
        // User destructor.
        symbol_kind_type yykind = this->kind ();
        basic_symbol<Base>& yysym = *this;
        (void) yysym;
        switch (yykind)
        {
       default:
          break;
        }

        // Value type destructor.
switch (yykind)
    {
      case symbol_kind::S_fn_call_args: // fn_call_args
        value.template destroy< shared_ptr<vector<shared_ptr<NExpression>>> > ();
        break;

      case symbol_kind::S_program: // program
      case symbol_kind::S_stmts: // stmts
      case symbol_kind::S_block: // block
        value.template destroy< std::shared_ptr<NBlock> > ();
        break;

      case symbol_kind::S_else_stmt: // else_stmt
        value.template destroy< std::shared_ptr<NElseStatement> > ();
        break;

      case symbol_kind::S_expr: // expr
      case symbol_kind::S_numeric: // numeric
      case symbol_kind::S_string: // string
        value.template destroy< std::shared_ptr<NExpression> > ();
        break;

      case symbol_kind::S_id: // id
        value.template destroy< std::shared_ptr<NIdentifier> > ();
        break;

      case symbol_kind::S_stmt: // stmt
      case symbol_kind::S_fn_decl: // fn_decl
      case symbol_kind::S_for_stmt: // for_stmt
      case symbol_kind::S_if_stmt: // if_stmt
      case symbol_kind::S_break_stmt: // break_stmt
      case symbol_kind::S_continue_stmt: // continue_stmt
        value.template destroy< std::shared_ptr<NStatement> > ();
        break;

      case symbol_kind::S_var_decl: // var_decl
        value.template destroy< std::shared_ptr<NVariableDeclaration> > ();
        break;

      case symbol_kind::S_TKNUMBER: // TKNUMBER
      case symbol_kind::S_TKDOUBLE: // TKDOUBLE
      case symbol_kind::S_TKSTRING: // TKSTRING
      case symbol_kind::S_TKPLUS: // TKPLUS
      case symbol_kind::S_TKMINUS: // TKMINUS
      case symbol_kind::S_TKMULT: // TKMULT
      case symbol_kind::S_TKDIV: // TKDIV
      case symbol_kind::S_TKMOD: // TKMOD
      case symbol_kind::S_TKRETURN: // TKRETURN
      case symbol_kind::S_TKCOMMA: // TKCOMMA
      case symbol_kind::S_TKDOT: // TKDOT
      case symbol_kind::S_TKARROW: // TKARROW
      case symbol_kind::S_TKCOLON: // TKCOLON
      case symbol_kind::S_TKRANGE_INCLUSIVE: // TKRANGE_INCLUSIVE
      case symbol_kind::S_TKLINEBREAK: // TKLINEBREAK
      case symbol_kind::S_TKASSIGNMENT: // TKASSIGNMENT
      case symbol_kind::S_TKLESS: // TKLESS
      case symbol_kind::S_TKGREATER: // TKGREATER
      case symbol_kind::S_TKLESS_EQUAL: // TKLESS_EQUAL
      case symbol_kind::S_TKGREATER_EQUAL: // TKGREATER_EQUAL
      case symbol_kind::S_TKEQUAL: // TKEQUAL
      case symbol_kind::S_TKNOT_EQUAL: // TKNOT_EQUAL
      case symbol_kind::S_TKAND: // TKAND
      case symbol_kind::S_TKOR: // TKOR
      case symbol_kind::S_TKNEGATION: // TKNEGATION
      case symbol_kind::S_TKIDENTIFIER: // TKIDENTIFIER
      case symbol_kind::S_TKDATATYPE: // TKDATATYPE
      case symbol_kind::S_TKCURLYOPEN: // TKCURLYOPEN
      case symbol_kind::S_TKCURLYCLOSE: // TKCURLYCLOSE
      case symbol_kind::S_TKPAROPEN: // TKPAROPEN
      case symbol_kind::S_TKPARCLOSE: // TKPARCLOSE
      case symbol_kind::S_TKFUNCTION_KEY: // TKFUNCTION_KEY
      case symbol_kind::S_TKIF: // TKIF
      case symbol_kind::S_TKELSE: // TKELSE
      case symbol_kind::S_TKELSEIF: // TKELSEIF
      case symbol_kind::S_TKFOR: // TKFOR
      case symbol_kind::S_TKIN: // TKIN
      case symbol_kind::S_TKBREAK: // TKBREAK
      case symbol_kind::S_TKCONT: // TKCONT
      case symbol_kind::S_TKINVALID: // TKINVALID
      case symbol_kind::S_binary_op: // binary_op
      case symbol_kind::S_negation: // negation
      case symbol_kind::S_comparison: // comparison
        value.template destroy< std::shared_ptr<Token> > ();
        break;

      case symbol_kind::S_fn_args: // fn_args
        value.template destroy< std::shared_ptr<VariableList> > ();
        break;

      default:
        break;
    }

        Base::clear ();
      }

      /// The user-facing name of this symbol.
      const char *name () const YY_NOEXCEPT
      {
        return fuse_parser::symbol_name (this->kind ());
      }

      /// Backward compatibility (Bison 3.6).
      symbol_kind_type type_get () const YY_NOEXCEPT;

      /// Whether empty.
      bool empty () const YY_NOEXCEPT;

      /// Destructive move, \a s is emptied into this.
      void move (basic_symbol& s);

      /// The semantic value.
      value_type value;

      /// The location.
      location_type location;

    private:
#if YY_CPLUSPLUS < 201103L
      /// Assignment operator.
      basic_symbol& operator= (const basic_symbol& that);
#endif
    };

    /// Type access provider for token (enum) based symbols.
    struct by_kind
    {
      /// The symbol kind as needed by the constructor.
      typedef token_kind_type kind_type;

      /// Default constructor.
      by_kind () YY_NOEXCEPT;

#if 201103L <= YY_CPLUSPLUS
      /// Move constructor.
      by_kind (by_kind&& that) YY_NOEXCEPT;
#endif

      /// Copy constructor.
      by_kind (const by_kind& that) YY_NOEXCEPT;

      /// Constructor from (external) token numbers.
      by_kind (kind_type t) YY_NOEXCEPT;



      /// Record that this symbol is empty.
      void clear () YY_NOEXCEPT;

      /// Steal the symbol kind from \a that.
      void move (by_kind& that);

      /// The (internal) type number (corresponding to \a type).
      /// \a empty when empty.
      symbol_kind_type kind () const YY_NOEXCEPT;

      /// Backward compatibility (Bison 3.6).
      symbol_kind_type type_get () const YY_NOEXCEPT;

      /// The symbol kind.
      /// \a S_YYEMPTY when empty.
      symbol_kind_type kind_;
    };

    /// Backward compatibility for a private implementation detail (Bison 3.6).
    typedef by_kind by_type;

    /// "External" symbols: returned by the scanner.
    struct symbol_type : basic_symbol<by_kind>
    {
      /// Superclass.
      typedef basic_symbol<by_kind> super_type;

      /// Empty symbol.
      symbol_type () YY_NOEXCEPT {}

      /// Constructor for valueless symbols, and symbols from each type.
#if 201103L <= YY_CPLUSPLUS
      symbol_type (int tok, location_type l)
        : super_type (token_kind_type (tok), std::move (l))
#else
      symbol_type (int tok, const location_type& l)
        : super_type (token_kind_type (tok), l)
#endif
      {}
#if 201103L <= YY_CPLUSPLUS
      symbol_type (int tok, std::shared_ptr<Token> v, location_type l)
        : super_type (token_kind_type (tok), std::move (v), std::move (l))
#else
      symbol_type (int tok, const std::shared_ptr<Token>& v, const location_type& l)
        : super_type (token_kind_type (tok), v, l)
#endif
      {}
    };

    /// Build a parser object.
    fuse_parser ();
    virtual ~fuse_parser ();

#if 201103L <= YY_CPLUSPLUS
    /// Non copyable.
    fuse_parser (const fuse_parser&) = delete;
    /// Non copyable.
    fuse_parser& operator= (const fuse_parser&) = delete;
#endif

    /// Parse.  An alias for parse ().
    /// \returns  0 iff parsing succeeded.
    int operator() ();

    /// Parse.
    /// \returns  0 iff parsing succeeded.
    virtual int parse ();

#if YYDEBUG
    /// The current debugging stream.
    std::ostream& debug_stream () const YY_ATTRIBUTE_PURE;
    /// Set the current debugging stream.
    void set_debug_stream (std::ostream &);

    /// Type for debugging levels.
    typedef int debug_level_type;
    /// The current debugging level.
    debug_level_type debug_level () const YY_ATTRIBUTE_PURE;
    /// Set the current debugging level.
    void set_debug_level (debug_level_type l);
#endif

    /// Report a syntax error.
    /// \param loc    where the syntax error is found.
    /// \param msg    a description of the syntax error.
    virtual void error (const location_type& loc, const std::string& msg);

    /// Report a syntax error.
    void error (const syntax_error& err);

    /// The user-facing name of the symbol whose (internal) number is
    /// YYSYMBOL.  No bounds checking.
    static const char *symbol_name (symbol_kind_type yysymbol);

    // Implementation of make_symbol for each token kind.
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_YYEOF (location_type l)
      {
        return symbol_type (token::YYEOF, std::move (l));
      }
#else
      static
      symbol_type
      make_YYEOF (const location_type& l)
      {
        return symbol_type (token::YYEOF, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_YYerror (location_type l)
      {
        return symbol_type (token::YYerror, std::move (l));
      }
#else
      static
      symbol_type
      make_YYerror (const location_type& l)
      {
        return symbol_type (token::YYerror, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_YYUNDEF (location_type l)
      {
        return symbol_type (token::YYUNDEF, std::move (l));
      }
#else
      static
      symbol_type
      make_YYUNDEF (const location_type& l)
      {
        return symbol_type (token::YYUNDEF, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TKNUMBER (std::shared_ptr<Token> v, location_type l)
      {
        return symbol_type (token::TKNUMBER, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_TKNUMBER (const std::shared_ptr<Token>& v, const location_type& l)
      {
        return symbol_type (token::TKNUMBER, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TKDOUBLE (std::shared_ptr<Token> v, location_type l)
      {
        return symbol_type (token::TKDOUBLE, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_TKDOUBLE (const std::shared_ptr<Token>& v, const location_type& l)
      {
        return symbol_type (token::TKDOUBLE, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TKSTRING (std::shared_ptr<Token> v, location_type l)
      {
        return symbol_type (token::TKSTRING, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_TKSTRING (const std::shared_ptr<Token>& v, const location_type& l)
      {
        return symbol_type (token::TKSTRING, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TKPLUS (std::shared_ptr<Token> v, location_type l)
      {
        return symbol_type (token::TKPLUS, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_TKPLUS (const std::shared_ptr<Token>& v, const location_type& l)
      {
        return symbol_type (token::TKPLUS, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TKMINUS (std::shared_ptr<Token> v, location_type l)
      {
        return symbol_type (token::TKMINUS, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_TKMINUS (const std::shared_ptr<Token>& v, const location_type& l)
      {
        return symbol_type (token::TKMINUS, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TKMULT (std::shared_ptr<Token> v, location_type l)
      {
        return symbol_type (token::TKMULT, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_TKMULT (const std::shared_ptr<Token>& v, const location_type& l)
      {
        return symbol_type (token::TKMULT, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TKDIV (std::shared_ptr<Token> v, location_type l)
      {
        return symbol_type (token::TKDIV, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_TKDIV (const std::shared_ptr<Token>& v, const location_type& l)
      {
        return symbol_type (token::TKDIV, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TKMOD (std::shared_ptr<Token> v, location_type l)
      {
        return symbol_type (token::TKMOD, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_TKMOD (const std::shared_ptr<Token>& v, const location_type& l)
      {
        return symbol_type (token::TKMOD, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TKRETURN (std::shared_ptr<Token> v, location_type l)
      {
        return symbol_type (token::TKRETURN, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_TKRETURN (const std::shared_ptr<Token>& v, const location_type& l)
      {
        return symbol_type (token::TKRETURN, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TKCOMMA (std::shared_ptr<Token> v, location_type l)
      {
        return symbol_type (token::TKCOMMA, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_TKCOMMA (const std::shared_ptr<Token>& v, const location_type& l)
      {
        return symbol_type (token::TKCOMMA, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TKDOT (std::shared_ptr<Token> v, location_type l)
      {
        return symbol_type (token::TKDOT, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_TKDOT (const std::shared_ptr<Token>& v, const location_type& l)
      {
        return symbol_type (token::TKDOT, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TKARROW (std::shared_ptr<Token> v, location_type l)
      {
        return symbol_type (token::TKARROW, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_TKARROW (const std::shared_ptr<Token>& v, const location_type& l)
      {
        return symbol_type (token::TKARROW, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TKCOLON (std::shared_ptr<Token> v, location_type l)
      {
        return symbol_type (token::TKCOLON, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_TKCOLON (const std::shared_ptr<Token>& v, const location_type& l)
      {
        return symbol_type (token::TKCOLON, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TKRANGE_INCLUSIVE (std::shared_ptr<Token> v, location_type l)
      {
        return symbol_type (token::TKRANGE_INCLUSIVE, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_TKRANGE_INCLUSIVE (const std::shared_ptr<Token>& v, const location_type& l)
      {
        return symbol_type (token::TKRANGE_INCLUSIVE, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TKLINEBREAK (std::shared_ptr<Token> v, location_type l)
      {
        return symbol_type (token::TKLINEBREAK, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_TKLINEBREAK (const std::shared_ptr<Token>& v, const location_type& l)
      {
        return symbol_type (token::TKLINEBREAK, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TKASSIGNMENT (std::shared_ptr<Token> v, location_type l)
      {
        return symbol_type (token::TKASSIGNMENT, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_TKASSIGNMENT (const std::shared_ptr<Token>& v, const location_type& l)
      {
        return symbol_type (token::TKASSIGNMENT, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TKLESS (std::shared_ptr<Token> v, location_type l)
      {
        return symbol_type (token::TKLESS, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_TKLESS (const std::shared_ptr<Token>& v, const location_type& l)
      {
        return symbol_type (token::TKLESS, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TKGREATER (std::shared_ptr<Token> v, location_type l)
      {
        return symbol_type (token::TKGREATER, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_TKGREATER (const std::shared_ptr<Token>& v, const location_type& l)
      {
        return symbol_type (token::TKGREATER, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TKLESS_EQUAL (std::shared_ptr<Token> v, location_type l)
      {
        return symbol_type (token::TKLESS_EQUAL, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_TKLESS_EQUAL (const std::shared_ptr<Token>& v, const location_type& l)
      {
        return symbol_type (token::TKLESS_EQUAL, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TKGREATER_EQUAL (std::shared_ptr<Token> v, location_type l)
      {
        return symbol_type (token::TKGREATER_EQUAL, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_TKGREATER_EQUAL (const std::shared_ptr<Token>& v, const location_type& l)
      {
        return symbol_type (token::TKGREATER_EQUAL, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TKEQUAL (std::shared_ptr<Token> v, location_type l)
      {
        return symbol_type (token::TKEQUAL, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_TKEQUAL (const std::shared_ptr<Token>& v, const location_type& l)
      {
        return symbol_type (token::TKEQUAL, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TKNOT_EQUAL (std::shared_ptr<Token> v, location_type l)
      {
        return symbol_type (token::TKNOT_EQUAL, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_TKNOT_EQUAL (const std::shared_ptr<Token>& v, const location_type& l)
      {
        return symbol_type (token::TKNOT_EQUAL, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TKAND (std::shared_ptr<Token> v, location_type l)
      {
        return symbol_type (token::TKAND, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_TKAND (const std::shared_ptr<Token>& v, const location_type& l)
      {
        return symbol_type (token::TKAND, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TKOR (std::shared_ptr<Token> v, location_type l)
      {
        return symbol_type (token::TKOR, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_TKOR (const std::shared_ptr<Token>& v, const location_type& l)
      {
        return symbol_type (token::TKOR, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TKNEGATION (std::shared_ptr<Token> v, location_type l)
      {
        return symbol_type (token::TKNEGATION, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_TKNEGATION (const std::shared_ptr<Token>& v, const location_type& l)
      {
        return symbol_type (token::TKNEGATION, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TKIDENTIFIER (std::shared_ptr<Token> v, location_type l)
      {
        return symbol_type (token::TKIDENTIFIER, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_TKIDENTIFIER (const std::shared_ptr<Token>& v, const location_type& l)
      {
        return symbol_type (token::TKIDENTIFIER, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TKDATATYPE (std::shared_ptr<Token> v, location_type l)
      {
        return symbol_type (token::TKDATATYPE, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_TKDATATYPE (const std::shared_ptr<Token>& v, const location_type& l)
      {
        return symbol_type (token::TKDATATYPE, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TKCURLYOPEN (std::shared_ptr<Token> v, location_type l)
      {
        return symbol_type (token::TKCURLYOPEN, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_TKCURLYOPEN (const std::shared_ptr<Token>& v, const location_type& l)
      {
        return symbol_type (token::TKCURLYOPEN, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TKCURLYCLOSE (std::shared_ptr<Token> v, location_type l)
      {
        return symbol_type (token::TKCURLYCLOSE, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_TKCURLYCLOSE (const std::shared_ptr<Token>& v, const location_type& l)
      {
        return symbol_type (token::TKCURLYCLOSE, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TKPAROPEN (std::shared_ptr<Token> v, location_type l)
      {
        return symbol_type (token::TKPAROPEN, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_TKPAROPEN (const std::shared_ptr<Token>& v, const location_type& l)
      {
        return symbol_type (token::TKPAROPEN, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TKPARCLOSE (std::shared_ptr<Token> v, location_type l)
      {
        return symbol_type (token::TKPARCLOSE, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_TKPARCLOSE (const std::shared_ptr<Token>& v, const location_type& l)
      {
        return symbol_type (token::TKPARCLOSE, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TKFUNCTION_KEY (std::shared_ptr<Token> v, location_type l)
      {
        return symbol_type (token::TKFUNCTION_KEY, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_TKFUNCTION_KEY (const std::shared_ptr<Token>& v, const location_type& l)
      {
        return symbol_type (token::TKFUNCTION_KEY, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TKIF (std::shared_ptr<Token> v, location_type l)
      {
        return symbol_type (token::TKIF, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_TKIF (const std::shared_ptr<Token>& v, const location_type& l)
      {
        return symbol_type (token::TKIF, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TKELSE (std::shared_ptr<Token> v, location_type l)
      {
        return symbol_type (token::TKELSE, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_TKELSE (const std::shared_ptr<Token>& v, const location_type& l)
      {
        return symbol_type (token::TKELSE, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TKELSEIF (std::shared_ptr<Token> v, location_type l)
      {
        return symbol_type (token::TKELSEIF, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_TKELSEIF (const std::shared_ptr<Token>& v, const location_type& l)
      {
        return symbol_type (token::TKELSEIF, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TKFOR (std::shared_ptr<Token> v, location_type l)
      {
        return symbol_type (token::TKFOR, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_TKFOR (const std::shared_ptr<Token>& v, const location_type& l)
      {
        return symbol_type (token::TKFOR, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TKIN (std::shared_ptr<Token> v, location_type l)
      {
        return symbol_type (token::TKIN, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_TKIN (const std::shared_ptr<Token>& v, const location_type& l)
      {
        return symbol_type (token::TKIN, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TKBREAK (std::shared_ptr<Token> v, location_type l)
      {
        return symbol_type (token::TKBREAK, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_TKBREAK (const std::shared_ptr<Token>& v, const location_type& l)
      {
        return symbol_type (token::TKBREAK, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TKCONT (std::shared_ptr<Token> v, location_type l)
      {
        return symbol_type (token::TKCONT, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_TKCONT (const std::shared_ptr<Token>& v, const location_type& l)
      {
        return symbol_type (token::TKCONT, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TKINVALID (std::shared_ptr<Token> v, location_type l)
      {
        return symbol_type (token::TKINVALID, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_TKINVALID (const std::shared_ptr<Token>& v, const location_type& l)
      {
        return symbol_type (token::TKINVALID, v, l);
      }
#endif


    class context
    {
    public:
      context (const fuse_parser& yyparser, const symbol_type& yyla);
      const symbol_type& lookahead () const YY_NOEXCEPT { return yyla_; }
      symbol_kind_type token () const YY_NOEXCEPT { return yyla_.kind (); }
      const location_type& location () const YY_NOEXCEPT { return yyla_.location; }

      /// Put in YYARG at most YYARGN of the expected tokens, and return the
      /// number of tokens stored in YYARG.  If YYARG is null, return the
      /// number of expected tokens (guaranteed to be less than YYNTOKENS).
      int expected_tokens (symbol_kind_type yyarg[], int yyargn) const;

    private:
      const fuse_parser& yyparser_;
      const symbol_type& yyla_;
    };

  private:
#if YY_CPLUSPLUS < 201103L
    /// Non copyable.
    fuse_parser (const fuse_parser&);
    /// Non copyable.
    fuse_parser& operator= (const fuse_parser&);
#endif


    /// Stored state numbers (used for stacks).
    typedef signed char state_type;

    /// The arguments of the error message.
    int yy_syntax_error_arguments_ (const context& yyctx,
                                    symbol_kind_type yyarg[], int yyargn) const;

    /// Generate an error message.
    /// \param yyctx     the context in which the error occurred.
    virtual std::string yysyntax_error_ (const context& yyctx) const;
    /// Compute post-reduction state.
    /// \param yystate   the current state
    /// \param yysym     the nonterminal to push on the stack
    static state_type yy_lr_goto_state_ (state_type yystate, int yysym);

    /// Whether the given \c yypact_ value indicates a defaulted state.
    /// \param yyvalue   the value to check
    static bool yy_pact_value_is_default_ (int yyvalue) YY_NOEXCEPT;

    /// Whether the given \c yytable_ value indicates a syntax error.
    /// \param yyvalue   the value to check
    static bool yy_table_value_is_error_ (int yyvalue) YY_NOEXCEPT;

    static const signed char yypact_ninf_;
    static const signed char yytable_ninf_;

    /// Convert a scanner token kind \a t to a symbol kind.
    /// In theory \a t should be a token_kind_type, but character literals
    /// are valid, yet not members of the token_kind_type enum.
    static symbol_kind_type yytranslate_ (int t) YY_NOEXCEPT;



    // Tables.
    // YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
    // STATE-NUM.
    static const short yypact_[];

    // YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
    // Performed when YYTABLE does not specify something else to do.  Zero
    // means the default is an error.
    static const signed char yydefact_[];

    // YYPGOTO[NTERM-NUM].
    static const signed char yypgoto_[];

    // YYDEFGOTO[NTERM-NUM].
    static const signed char yydefgoto_[];

    // YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
    // positive, shift that token.  If negative, reduce the rule whose
    // number is the opposite.  If YYTABLE_NINF, syntax error.
    static const signed char yytable_[];

    static const signed char yycheck_[];

    // YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
    // state STATE-NUM.
    static const signed char yystos_[];

    // YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.
    static const signed char yyr1_[];

    // YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.
    static const signed char yyr2_[];


#if YYDEBUG
    // YYRLINE[YYN] -- Source line where rule number YYN was defined.
    static const short yyrline_[];
    /// Report on the debug stream that the rule \a r is going to be reduced.
    virtual void yy_reduce_print_ (int r) const;
    /// Print the state stack on the debug stream.
    virtual void yy_stack_print_ () const;

    /// Debugging level.
    int yydebug_;
    /// Debug stream.
    std::ostream* yycdebug_;

    /// \brief Display a symbol kind, value and location.
    /// \param yyo    The output stream.
    /// \param yysym  The symbol.
    template <typename Base>
    void yy_print_ (std::ostream& yyo, const basic_symbol<Base>& yysym) const;
#endif

    /// \brief Reclaim the memory associated to a symbol.
    /// \param yymsg     Why this token is reclaimed.
    ///                  If null, print nothing.
    /// \param yysym     The symbol.
    template <typename Base>
    void yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const;

  private:
    /// Type access provider for state based symbols.
    struct by_state
    {
      /// Default constructor.
      by_state () YY_NOEXCEPT;

      /// The symbol kind as needed by the constructor.
      typedef state_type kind_type;

      /// Constructor.
      by_state (kind_type s) YY_NOEXCEPT;

      /// Copy constructor.
      by_state (const by_state& that) YY_NOEXCEPT;

      /// Record that this symbol is empty.
      void clear () YY_NOEXCEPT;

      /// Steal the symbol kind from \a that.
      void move (by_state& that);

      /// The symbol kind (corresponding to \a state).
      /// \a symbol_kind::S_YYEMPTY when empty.
      symbol_kind_type kind () const YY_NOEXCEPT;

      /// The state number used to denote an empty symbol.
      /// We use the initial state, as it does not have a value.
      enum { empty_state = 0 };

      /// The state.
      /// \a empty when empty.
      state_type state;
    };

    /// "Internal" symbol: element of the stack.
    struct stack_symbol_type : basic_symbol<by_state>
    {
      /// Superclass.
      typedef basic_symbol<by_state> super_type;
      /// Construct an empty symbol.
      stack_symbol_type ();
      /// Move or copy construction.
      stack_symbol_type (YY_RVREF (stack_symbol_type) that);
      /// Steal the contents from \a sym to build this.
      stack_symbol_type (state_type s, YY_MOVE_REF (symbol_type) sym);
#if YY_CPLUSPLUS < 201103L
      /// Assignment, needed by push_back by some old implementations.
      /// Moves the contents of that.
      stack_symbol_type& operator= (stack_symbol_type& that);

      /// Assignment, needed by push_back by other implementations.
      /// Needed by some other old implementations.
      stack_symbol_type& operator= (const stack_symbol_type& that);
#endif
    };

    /// A stack with random access from its top.
    template <typename T, typename S = std::vector<T> >
    class stack
    {
    public:
      // Hide our reversed order.
      typedef typename S::iterator iterator;
      typedef typename S::const_iterator const_iterator;
      typedef typename S::size_type size_type;
      typedef typename std::ptrdiff_t index_type;

      stack (size_type n = 200) YY_NOEXCEPT
        : seq_ (n)
      {}

#if 201103L <= YY_CPLUSPLUS
      /// Non copyable.
      stack (const stack&) = delete;
      /// Non copyable.
      stack& operator= (const stack&) = delete;
#endif

      /// Random access.
      ///
      /// Index 0 returns the topmost element.
      const T&
      operator[] (index_type i) const
      {
        return seq_[size_type (size () - 1 - i)];
      }

      /// Random access.
      ///
      /// Index 0 returns the topmost element.
      T&
      operator[] (index_type i)
      {
        return seq_[size_type (size () - 1 - i)];
      }

      /// Steal the contents of \a t.
      ///
      /// Close to move-semantics.
      void
      push (YY_MOVE_REF (T) t)
      {
        seq_.push_back (T ());
        operator[] (0).move (t);
      }

      /// Pop elements from the stack.
      void
      pop (std::ptrdiff_t n = 1) YY_NOEXCEPT
      {
        for (; 0 < n; --n)
          seq_.pop_back ();
      }

      /// Pop all elements from the stack.
      void
      clear () YY_NOEXCEPT
      {
        seq_.clear ();
      }

      /// Number of elements on the stack.
      index_type
      size () const YY_NOEXCEPT
      {
        return index_type (seq_.size ());
      }

      /// Iterator on top of the stack (going downwards).
      const_iterator
      begin () const YY_NOEXCEPT
      {
        return seq_.begin ();
      }

      /// Bottom of the stack.
      const_iterator
      end () const YY_NOEXCEPT
      {
        return seq_.end ();
      }

      /// Present a slice of the top of a stack.
      class slice
      {
      public:
        slice (const stack& stack, index_type range) YY_NOEXCEPT
          : stack_ (stack)
          , range_ (range)
        {}

        const T&
        operator[] (index_type i) const
        {
          return stack_[range_ - i];
        }

      private:
        const stack& stack_;
        index_type range_;
      };

    private:
#if YY_CPLUSPLUS < 201103L
      /// Non copyable.
      stack (const stack&);
      /// Non copyable.
      stack& operator= (const stack&);
#endif
      /// The wrapped container.
      S seq_;
    };


    /// Stack type.
    typedef stack<stack_symbol_type> stack_type;

    /// The stack.
    stack_type yystack_;

    /// Push a new state on the stack.
    /// \param m    a debug message to display
    ///             if null, no trace is output.
    /// \param sym  the symbol
    /// \warning the contents of \a s.value is stolen.
    void yypush_ (const char* m, YY_MOVE_REF (stack_symbol_type) sym);

    /// Push a new look ahead token on the state on the stack.
    /// \param m    a debug message to display
    ///             if null, no trace is output.
    /// \param s    the state
    /// \param sym  the symbol (for its value and location).
    /// \warning the contents of \a sym.value is stolen.
    void yypush_ (const char* m, state_type s, YY_MOVE_REF (symbol_type) sym);

    /// Pop \a n symbols from the stack.
    void yypop_ (int n = 1) YY_NOEXCEPT;

    /// Constants.
    enum
    {
      yylast_ = 419,     ///< Last index in yytable_.
      yynnts_ = 21,  ///< Number of nonterminal symbols.
      yyfinal_ = 43 ///< Termination state number.
    };



  };


} // yy
#line 2157 "/Users/davidfeliperiveraguerra/dev/Interfuse-Project/compiler/include/parser.hpp"




#endif // !YY_YY_USERS_DAVIDFELIPERIVERAGUERRA_DEV_INTERFUSE_PROJECT_COMPILER_INCLUDE_PARSER_HPP_INCLUDED
