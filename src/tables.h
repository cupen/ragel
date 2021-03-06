/*
 * Copyright 2018 Adrian Thurston <thurston@colm.net>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef _TABLES_H
#define _TABLES_H

#include <iostream>
#include "codegen.h"

struct Tables
:
	public CodeGen
{
	Tables( const CodeGenArgs &args ) 
	:
		CodeGen( args ),

		pa( "_pa" ),
		klen( "_klen" ),
		ckeys( "_ckeys" ),
		cekeys( "_cekeys" ),
		trans( "_trans" ),
		cond( "_cond" ),
		keys( "_keys" ),
		acts( "_acts" ),
		nacts( "_nacts" ),
		inds( "_inds" ),

		actions(          "actions",             *this ),
		transKeys(        "trans_keys",          *this ),
		charClass(        "char_class",          *this ),
		flatIndexOffset(  "index_offsets",       *this ),
		indicies(         "indicies",            *this ),
		indexDefaults(    "index_defaults",      *this ),
		transCondSpaces(  "trans_cond_spaces",   *this ),
		transOffsets(     "trans_offsets",       *this ),
		condTargs(        "cond_targs",          *this ),
		condActions(      "cond_actions",        *this ),
		toStateActions(   "to_state_actions",    *this ),
		fromStateActions( "from_state_actions",  *this ),
		eofCondSpaces(    "eof_cond_spaces",     *this ),
		eofCondKeyOffs(   "eof_cond_key_offs",   *this ),
		eofCondKeyLens(   "eof_cond_key_lens",   *this ),
		eofCondKeys(      "eof_cond_keys",       *this ),
		eofActions(       "eof_actions",         *this ),
		eofTrans(         "eof_trans",           *this ),

		keyOffsets(         "key_offsets",           *this ),
		singleLens(         "single_lengths",        *this ),
		rangeLens(          "range_lengths",         *this ),
		indexOffsets(       "index_offsets",         *this ),
		transCondSpacesWi(  "trans_cond_spaces_wi",  *this ),
		transOffsetsWi(     "trans_offsets_wi",      *this ),
		transLengthsWi(     "trans_lengths_wi",      *this ),
		transLengths(       "trans_lengths",         *this ),
		condKeys(           "cond_keys",             *this )
	{}

	Variable pa;
	Variable klen;
	Variable ckeys;
	Variable cekeys;
	Variable trans;
	Variable cond;
	Variable keys;
	Variable acts;
	Variable nacts;
	Variable inds;

	TableArray actions;
	TableArray transKeys;
	TableArray charClass;
	TableArray flatIndexOffset;
	TableArray indicies;
	TableArray indexDefaults;
	TableArray transCondSpaces;
	TableArray transOffsets;
	TableArray condTargs;
	TableArray condActions;
	TableArray toStateActions;
	TableArray fromStateActions;
	TableArray eofCondSpaces;
	TableArray eofCondKeyOffs;
	TableArray eofCondKeyLens;
	TableArray eofCondKeys;
	TableArray eofActions;
	TableArray eofTrans;

	TableArray keyOffsets;
	TableArray singleLens;
	TableArray rangeLens;
	TableArray indexOffsets;
	TableArray transCondSpacesWi;
	TableArray transOffsetsWi;
	TableArray transLengthsWi;
	TableArray transLengths;
	TableArray condKeys;

	virtual void TO_STATE_ACTION( RedStateAp *state ) = 0;
	virtual void FROM_STATE_ACTION( RedStateAp *state ) = 0;
	virtual void EOF_ACTION( RedStateAp *state ) = 0;
	virtual void COND_ACTION( RedCondPair *cond ) = 0;

	virtual void NFA_PUSH_ACTION( RedNfaTarg *targ ) = 0;
	virtual void NFA_POP_TEST( RedNfaTarg *targ ) = 0;
	virtual void NFA_FROM_STATE_ACTION_EXEC() = 0;

	virtual void FROM_STATE_ACTIONS() = 0;
	virtual void REG_ACTIONS( std::string cond ) = 0;
	virtual void TO_STATE_ACTIONS() = 0;
	virtual void EOF_ACTIONS() = 0;

	void CURS( ostream &ret, bool inFinish );
	void TARGS( ostream &ret, bool inFinish, int targState );
	void NEXT( ostream &ret, int nextDest, bool inFinish );
	void NEXT_EXPR( ostream &ret, GenInlineItem *ilItem, bool inFinish );
	void EOF_TRANS();
	void COND_EXEC( std::string expr );
};

struct TabGoto
:
	public virtual Tables
{
	TabGoto( const CodeGenArgs &args )
	:
		Tables( args )
	{}

	void GOTO( ostream &ret, int gotoDest, bool inFinish );
	void GOTO_EXPR( ostream &ret, GenInlineItem *ilItem, bool inFinish );
	void CALL( ostream &ret, int callDest, int targState, bool inFinish );
	void NCALL( ostream &ret, int callDest, int targState, bool inFinish );
	void CALL_EXPR( ostream &ret, GenInlineItem *ilItem, int targState, bool inFinish );
	void NCALL_EXPR( ostream &ret, GenInlineItem *ilItem, int targState, bool inFinish );
	void RET( ostream &ret, bool inFinish );
	void NRET( ostream &ret, bool inFinish );
	void BREAK( ostream &ret, int targState, bool csForced );
	void NBREAK( ostream &ret, int targState, bool csForced );

	void NFA_POP();
	void writeExec();
};

struct TabVar
:
	public virtual Tables
{
	TabVar( const CodeGenArgs &args )
	:
		Tables( args )
	{}

	void GOTO( ostream &ret, int gotoDest, bool inFinish );
	void GOTO_EXPR( ostream &ret, GenInlineItem *ilItem, bool inFinish );
	void CALL( ostream &ret, int callDest, int targState, bool inFinish );
	void NCALL( ostream &ret, int callDest, int targState, bool inFinish );
	void CALL_EXPR( ostream &ret, GenInlineItem *ilItem, int targState, bool inFinish );
	void NCALL_EXPR( ostream &ret, GenInlineItem *ilItem, int targState, bool inFinish );
	void RET( ostream &ret, bool inFinish );
	void NRET( ostream &ret, bool inFinish );
	void BREAK( ostream &ret, int targState, bool csForced );
	void NBREAK( ostream &ret, int targState, bool csForced );

	void NFA_POP();
	void writeExec();
};


#endif
