/*
 * Copyright 2018-2018 Adrian Thurston <thurston@colm.net>
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

#include "flatgoto.h"

void FlatGoto::COND_BIN_SEARCH( Variable &var, TableArray &keys, std::string ok, std::string error )
{
	out <<
		"	{\n"
		"		" << INDEX( ARR_TYPE( keys ), "_lower" ) << " = " << var << ";\n"
		"		" << INDEX( ARR_TYPE( keys ), "_upper" ) << " = " << var << " + " << klen << " - 1;\n"
		"		" << INDEX( ARR_TYPE( keys ), "_mid" ) << ";\n"
		"		while ( " << TRUE() << " ) {\n"
		"			if ( _upper < _lower )\n"
		"				break;\n"
		"\n"
		"			_mid = _lower + ((_upper-_lower) >> 1);\n"
		"			if ( " << cpc << " < " << CAST("int") << DEREF( ARR_REF( keys ), "_mid" ) << " )\n"
		"				_upper = _mid - 1;\n"
		"			else if ( " << cpc << " > " << CAST( "int" ) << DEREF( ARR_REF( keys ), "_mid" ) << " )\n"
		"				_lower = _mid + 1;\n"
		"			else {\n"
		"				" << ok << "\n"
		"			}\n"
		"		}\n"
		"		" << vCS() << " = " << ERROR_STATE() << ";\n"
		"		" << error << "\n"
		"	}\n"
	;
}

void FlatGoto::LOCATE_TRANS()
{
	if ( redFsm->classMap == 0 ) {
		out <<
			"	" << trans << " = " << CAST(UINT()) << ARR_REF( indexDefaults ) << "[" << vCS() << "]" << ";\n";
	}
	else {
		long lowKey = redFsm->lowKey.getVal();
		long highKey = redFsm->highKey.getVal();

		bool limitLow = keyOps->eq( lowKey, keyOps->minKey );
		bool limitHigh = keyOps->eq( highKey, keyOps->maxKey );

		out <<
			"	" << keys << " = " << OFFSET( ARR_REF( transKeys ), "(" + vCS() + "<<1)" ) << ";\n"
			"	" << inds << " = " << OFFSET( ARR_REF( indicies ),
					ARR_REF( flatIndexOffset ) + "[" + vCS() + "]" ) << ";\n"
			"\n";

		if ( !limitLow || !limitHigh ) {
			out << "	if ( ";

			if ( !limitHigh )
				out << GET_KEY() << " <= " << highKey;

			if ( !limitHigh && !limitLow )
				out << " && ";

			if ( !limitLow )
				out << GET_KEY() << " >= " << lowKey;

			out << " )\n	{\n";
		}

		out <<
			"       int _ic = " << CAST("int") << ARR_REF( charClass ) << "[" << CAST("int") << GET_KEY() <<
							" - " << lowKey << "];\n"
			"		if ( _ic <= " << CAST("int") << DEREF( ARR_REF( transKeys ), string(keys) + "+1" ) << " && " <<
						"_ic >= " << CAST("int") << DEREF( ARR_REF( transKeys ), string(keys) + "" ) << " )\n"
			"			" << trans << " = " << CAST(UINT()) << DEREF( ARR_REF( indicies ),
								string(inds) + " + " + CAST("int") + "( _ic - " + CAST("int") +
								DEREF( ARR_REF( transKeys ), string(keys) + "" ) + " ) " ) << "; \n"
			"		else\n"
			"			" << trans << " = " << CAST(UINT()) << ARR_REF( indexDefaults ) <<
								"[" << vCS() << "]" << ";\n";

		if ( !limitLow || !limitHigh ) {
			out <<
				"	}\n"
				"	else {\n"
				"		" << trans << " = " << CAST(UINT()) << ARR_REF( indexDefaults ) << "[" << vCS() << "]" << ";\n"
				"	}\n"
				"\n";
		}
	}


	if ( red->condSpaceList.length() > 0 ) {
		out <<
			"	" << cond << " = " << CAST( UINT() ) << ARR_REF( transOffsets ) << "[" << trans << "];\n"
			"\n";

		out <<
			"	" << cpc << " = 0;\n";

		out <<
			"	switch ( " << ARR_REF( transCondSpaces ) << "[" << trans << "] ) {\n"
			"\n";

		for ( CondSpaceList::Iter csi = red->condSpaceList; csi.lte(); csi++ ) {
			GenCondSpace *condSpace = csi;
			if ( condSpace->numTransRefs > 0 ) {
				out << "	" << CASE( STR(condSpace->condSpaceId) ) << " {\n";
				for ( GenCondSet::Iter csi = condSpace->condSet; csi.lte(); csi++ ) {
					out << TABS(2) << "if ( ";
					CONDITION( out, *csi );
					Size condValOffset = (1 << csi.pos());
					out << " ) " << cpc << " += " << condValOffset << ";\n";
				}

				out << 
					"	" << CEND() << "}\n";
			}
		}

		out << 
			"	}\n"
			"	" << cond << " += " << CAST( UINT() ) << "" << cpc << ";\n";
	}
	
	out <<
		"	goto _match_cond;\n"
	;
}
