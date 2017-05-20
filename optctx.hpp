/**********************************************************\
 *   Small library for managing cmd options with contexts
 * *********************************************************
 * Copyright © Félix Faisant 2013-2014. All rights reserved
 * This software is under the GNU General Public License
 \**********************************************************/

#ifdef OPTCTX_IMPL
	namespace optctx {
		bool interactive = true;
	}
#else
	namespace optctx {
		extern bool interactive;
	}
#endif

#ifdef OPTCTX_IMPL

#include <stdlib.h>
#include <getopt.h>
#include <stack>

#ifndef OPTCTX_POSTFNCT_EXCEPT_T
	#error Exception catching type for post-context function must be defined with OPTCTX_POSTFNCT_EXCEPT_T
	#ifndef OPTCTX_POSTFNCT_EXCEPT_DEFAULT
		#error Default exception value for indicating "no error" to post-context function must be defined with OPTCTX_POSTFNCT_EXCEPT_DEFAULT
	#endif
#endif

typedef OPTCTX_POSTFNCT_EXCEPT_T ctx_postfnct_excpt_t;
ctx_postfnct_excpt_t ctx_postfnct_excpt_default = OPTCTX_POSTFNCT_EXCEPT_DEFAULT;

#ifndef OPTCTX_PROG_NAME
	#error Program name must be defined with OPTCTX_PROG_NAME
#endif

inline __attribute__ ((noreturn)) void try_help () {
	if (optctx::interactive)
		::fputs("Try '" OPTCTX_PROG_NAME" --help' for more information.\n", stderr);
	::exit(2);
}
inline __attribute__ ((noreturn)) void try_help (const char* info) {
	::fputs(info, stderr);
	try_help();
}

#if !defined(OPTCTX_CTXS) || !defined(OPTCTX_PARENTS) || !defined(OPTCTX_PARENTS_NAMES) || !defined(OPTCTX_PARENTS_FNCTS) || !defined(OPTCTX_NAMES)
	#error OPTCTX_CTXS, OPTCTX_PARENTS, OPTCTX_PARENTS_NAMES, OPTCTX_PARENTS_FNCTS and OPTCTX_NAMES lists must be defined
#endif

namespace optctx {
	struct ctxfncts_t { void (*pre)(void); void (*op)(void); void (*post)(ctx_postfnct_excpt_t); };
	#define CTXFP(pre,post) { pre, NULL, post }
	#define CTXFO(op) { NULL, op, NULL }
	enum optctx_t                 { ROOT = 0, OPTCTX_CTXS    } optctx = ROOT;
	optctx_t optctx_tree[] =      { ROOT    , OPTCTX_PARENTS };
	const char* optctx_names[] =  { OPTCTX_PARENTS_NAMES     };
	ctxfncts_t optctx_fnct [] =   { OPTCTX_PARENTS_FNCTS     };
	const char* optctx_optnm[] =  { NULL, OPTCTX_NAMES       };
	inline void optctx_test (const char* optname, optctx_t parent_ctx) {
		optctx_t rctx = optctx;
		do {
			if (rctx == parent_ctx) 
				return;
		} while ((rctx = optctx_tree[rctx]) != optctx_t::ROOT);
		::fprintf(stderr, "%s : need %s to be triggered first\n", optname, optctx_optnm[parent_ctx]);
		try_help();
	}
	inline void optctx_set (optctx_t new_ctx) {
		optctx_t rctx = optctx;
		size_t optctx_level = 0;
		while (rctx != optctx_t::ROOT) {
			rctx = optctx_tree[rctx];
			optctx_level++;
		}
		rctx = new_ctx;
		size_t newctx_level = 0;
		while (rctx != optctx_t::ROOT) {
			rctx = optctx_tree[rctx];
			newctx_level++;
		}
		if (new_ctx == optctx) {
			::fprintf(stderr, "%s already triggered\n", optctx_optnm[new_ctx]);
			try_help();
		}
		if (optctx_level < newctx_level-1) {
			::fprintf(stderr, "%s : need %s to be triggered first\n", optctx_optnm[new_ctx], optctx_optnm[optctx_tree[new_ctx]]);
			try_help();
		}
		if (newctx_level-1 == optctx_level && optctx != optctx_tree[new_ctx]) {
			::fprintf(stderr, "%s : need %s to be triggered, but another %s triggered\n", optctx_optnm[new_ctx], optctx_optnm[optctx_tree[new_ctx]], optctx_names[optctx_tree[optctx]]);
			try_help();
		}
		if (newctx_level == optctx_level) {
			::fprintf(stderr, "%s : another %s already triggered\n", optctx_optnm[new_ctx], optctx_names[optctx_tree[optctx]]);
			try_help();
		}
		if (optctx_level > newctx_level) {
			::fprintf(stderr, "%s : can't be used in this context\n", optctx_optnm[new_ctx]);
			try_help();
		}
		optctx = new_ctx;
	}
	inline void optctx_end () {
		if (optctx_names[optctx] != NULL) {
			::fprintf(stderr, "No %s triggered !\n", optctx_names[optctx]);
			try_help();
		}
	}
	inline void _optctx_exec_rec (std::stack<ctxfncts_t> fncts_stack) {
		if (fncts_stack.empty()) return;
		ctxfncts_t fncts = fncts_stack.top();
		fncts_stack.pop();
		if (fncts.pre != NULL)
			(*fncts.pre)();
		try {
			if (fncts.op != NULL)
				(*fncts.op)();
			_optctx_exec_rec(fncts_stack);
		} catch (ctx_postfnct_excpt_t& e) {
			if (fncts.post != NULL)
				(*fncts.post)(e);
			else throw e;
			return;
		}
		if (fncts.post != NULL)
			(*fncts.post)(ctx_postfnct_excpt_default);
	}
	inline void optctx_exec () {
		std::stack<ctxfncts_t> fncts_stack;
		optctx_t rctx = optctx;
		fncts_stack.push(optctx_fnct[rctx]);
		while (rctx != optctx_t::ROOT) {
			rctx = optctx_tree[rctx];
			fncts_stack.push(optctx_fnct[rctx]);
		}
		optctx::_optctx_exec_rec(fncts_stack);
	}
}

#endif
