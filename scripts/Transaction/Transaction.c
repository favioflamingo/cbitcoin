/*
 * This file was generated automatically by ExtUtils::ParseXS version 2.2210 from the
 * contents of Transaction.xs. Do not edit this file, edit Transaction.xs instead.
 *
 *	ANY CHANGES MADE HERE WILL BE LOST! 
 *
 */

#line 1 "Transaction.xs"
#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

#include <stdio.h>
#include <ctype.h>
#include <openssl/ssl.h>
#include <openssl/ripemd.h>
#include <openssl/rand.h>
#include <CBHDKeys.h>
#include <CBChecksumBytes.h>
#include <CBAddress.h>
#include <CBWIF.h>
#include <CBByteArray.h>
#include <CBBase58.h>
#include <CBScript.h>
#include <CBTransaction.h>
#include <CBTransactionInput.h>
#include <CBTransactionOutput.h>


// print CBByteArray to hex string
char* bytearray_to_hexstring(CBByteArray * serializeddata,uint32_t dlen){
	char* answer = malloc(dlen*sizeof(char*));
	CBByteArrayToString(serializeddata, 0, dlen, answer, 0);
	return answer;
}
CBByteArray* hexstring_to_bytearray(char* hexstring){
	CBByteArray* answer = CBNewByteArrayFromHex(hexstring);
	return answer;
}

//bool CBInitScriptFromString(CBScript * self, char * string)
char* scriptToString(CBScript* script){
	char* answer = (char *)malloc(CBScriptStringMaxSize(script)*sizeof(char));
	CBScriptToString(script, answer);
	return answer;

}


CBTransaction* serializeddata_to_obj(char* datastring){

	CBByteArray* data = hexstring_to_bytearray(datastring);

	CBTransaction* tx = CBNewTransactionFromData(data);
	uint32_t dlen = CBTransactionDeserialise(tx);

	//CBDestroyByteArray(data);
	return tx;
}

char* obj_to_serializeddata(CBTransaction * tx){
	CBTransactionPrepareBytes(tx);
	int dlen = CBTransactionSerialise(tx,1);
	CBByteArray* serializeddata = CBGetMessage(tx)->bytes;

	char* answer = bytearray_to_hexstring(serializeddata,dlen);

	return answer;
}

/*
 * TransactionOutput related functions
 */
char* txoutput_obj_to_serializeddata(CBTransactionOutput * txoutput){
	CBTransactionOutputPrepareBytes(txoutput);
	int dlen = CBTransactionOutputSerialise(txoutput);
	CBByteArray* serializeddata = CBGetMessage(txoutput)->bytes;

	char* answer = bytearray_to_hexstring(serializeddata,dlen);

	return answer;
}
CBTransactionOutput* txoutput_serializeddata_to_obj(char* datastring){

	CBByteArray* data = hexstring_to_bytearray(datastring);

	CBTransactionOutput* txoutput = CBNewTransactionOutputFromData(data);
	int dlen = (int)CBTransactionOutputDeserialise(txoutput);

	//CBTransactionInputDeserialise(txinput);
	//CBDestroyByteArray(data);
	return txoutput;
}
/*
 * TransactionInput related functions
 */

CBTransactionInput* txinput_serializeddata_to_obj(char* datastring){

	CBByteArray* data = hexstring_to_bytearray(datastring);

	CBTransactionInput* txinput = CBNewTransactionInputFromData(data);
	int dlen = (int)CBTransactionInputDeserialise(txinput);

	//CBTransactionInputDeserialise(txinput);
	//CBDestroyByteArray(data);
	return txinput;
}

char* txinput_obj_to_serializeddata(CBTransactionInput * txinput){
	CBTransactionInputPrepareBytes(txinput);
	int dlen = CBTransactionInputSerialise(txinput);
	CBByteArray* serializeddata = CBGetMessage(txinput)->bytes;

	char* answer = bytearray_to_hexstring(serializeddata,dlen);

	return answer;
}

// CBHDKeys

CBHDKey* cbhdkey_serializeddata_to_obj(char* privstring){
	CBByteArray * masterString = CBNewByteArrayFromString(privstring, true);
	CBChecksumBytes * masterData = CBNewChecksumBytesFromString(masterString, false);
	CBReleaseObject(masterString);
	CBHDKey * masterkey = CBNewHDKeyFromData(CBByteArrayGetData(CBGetByteArray(masterData)));
	CBReleaseObject(masterData);
	return (CBHDKey *)masterkey;
}

char* cbhdkey_obj_to_serializeddata(CBHDKey * keypair){
	uint8_t * keyData = malloc(CB_HD_KEY_STR_SIZE);
	CBHDKeySerialise(keypair, keyData);

	CBChecksumBytes * checksumBytes = CBNewChecksumBytesFromBytes(keyData, 82, false);
	// need to figure out how to free keyData memory
	CBByteArray * str = CBChecksumBytesGetString(checksumBytes);
	CBReleaseObject(checksumBytes);
	return (char *)CBByteArrayGetData(str);
}
/*
 *  CBScript
 */
char* script_obj_to_serializeddata(CBScript* script){
	char* answer = (char *)malloc(CBScriptStringMaxSize(script)*sizeof(char));
	CBScriptToString(script, answer);
	return answer;

}
CBScript* script_serializeddata_to_obj(char* scriptstring){
	CBScript* self;
	if(CBInitScriptFromString(self,scriptstring)){
		return self;
	}
	else{
		return NULL;
	}
}

//////////////////////// perl export functions /////////////
//CBTransactionInput * CBNewTransactionInput(CBScript * script, uint32_t sequence, CBByteArray * prevOutHash, uint32_t prevOutIndex)

char* create_tx_obj(int lockTime, int version, SV* inputs, SV* outputs, int numOfInputs, int numOfOutputs){
	CBTransaction* tx = CBNewTransaction((uint32_t) lockTime, (uint32_t) version);

	int n;
	int in_length, out_length;
    if ((! SvROK(inputs))
    || (SvTYPE(SvRV(inputs)) != SVt_PVAV)
    || ((in_length = av_len((AV *)SvRV(inputs))) < 0))
    {
        return 0;
    }
    if ((! SvROK(outputs))
    || (SvTYPE(SvRV(outputs)) != SVt_PVAV)
    || ((out_length = av_len((AV *)SvRV(outputs))) < 0))
    {
        return 0;
    }

    // load TransactionInput
	for (n=0; n<=in_length; n++) {
		STRLEN l;

		char * fn = SvPV (*av_fetch ((AV *) SvRV (inputs), n, 0), l);
		CBTransactionInput * inx = txinput_serializeddata_to_obj(fn);
		CBTransactionAddInput(tx,inx);
	}
	for (n=0; n<=out_length; n++) {
		STRLEN l;

		char * fn = SvPV (*av_fetch ((AV *) SvRV (outputs), n, 0), l);
		CBTransactionOutput * outx = txoutput_serializeddata_to_obj(fn);
		CBTransactionAddOutput(tx,outx);
	}
	char* answer = obj_to_serializeddata(tx);
	//CBFreeTransaction(tx);
	return answer;
}
/*
char* get_script_from_obj(char* serializedDataString){
	CBTransactionOutput* txoutput = serializeddata_to_obj(serializedDataString);
	char* scriptstring = scriptToString(txoutput->scriptObject);
	//CBFreeTransactionOutput(txoutput);
	return scriptstring;
}
*/
int get_numOfInputs(char* serializedDataString){
	CBTransaction* tx = serializeddata_to_obj(serializedDataString);
	uint32_t numOfInputs = tx->inputNum;
	CBFreeTransaction(tx);
	return (int)numOfInputs;	
}
int get_numOfOutputs(char* serializedDataString){
	CBTransaction* tx = serializeddata_to_obj(serializedDataString);
	uint32_t numOfOutputs = tx->outputNum;
	CBFreeTransaction(tx);
	return (int)numOfOutputs;
}
char* get_Input(char* serializedDataString,int InputIndex){
	CBTransaction* tx = serializeddata_to_obj(serializedDataString);
	CBTransactionInput** inputs = tx->inputs;
	char* answer = txinput_obj_to_serializeddata(inputs[InputIndex]);
	CBFreeTransaction(tx);
	return answer;
}
char* get_Output(char* serializedDataString,int OutputIndex){
	CBTransaction* tx = serializeddata_to_obj(serializedDataString);
	CBTransactionOutput** outputs = tx->outputs;
	char* answer = txoutput_obj_to_serializeddata(outputs[OutputIndex]);
	CBFreeTransaction(tx);
	return answer;
}

char* hash_of_tx(char* serializedDataString){
	CBTransaction* tx = serializeddata_to_obj(serializedDataString);
	CBByteArray * data = CBNewByteArrayWithData(CBTransactionGetHash(tx), (uint32_t)32);
	CBFreeTransaction(tx);
	return bytearray_to_hexstring(data,32);
}

int get_lockTime_from_obj(char* serializedDataString){
	CBTransaction* tx = serializeddata_to_obj(serializedDataString);
	uint32_t lockTime = tx->lockTime;
	CBFreeTransaction(tx);
	return (int)lockTime;
}

int get_version_from_obj(char* serializedDataString){
	CBTransaction* tx = serializeddata_to_obj(serializedDataString);
	uint32_t version = tx->version;
	CBFreeTransaction(tx);
	return (int)version;
}
// CBTransaction * self, CBKeyPair * key, CBByteArray * prevOutSubScript, uint32_t input, CBSignType signType
char* sign_tx_pubkeyhash(char* txString, char* keypairString, char* prevOutSubScriptString, int input, char* signTypeString){
        //CBLogError("sign 0.");
	//fprintf(stderr, "sign 0.");
	CBTransaction * tx = serializeddata_to_obj(txString);
        //fprintf(stderr, "sign 0.1");
	CBHDKey * keypair = cbhdkey_serializeddata_to_obj(keypairString);
        //fprintf(stderr, "sign 0.2");
	//CBScript * prevOutSubScript = script_serializeddata_to_obj(prevOutSubScriptString);
	CBScript * prevOutSubScript = tx->inputs[input]->scriptObject;
	//CBLogError("sign 1.");
	// figure out the signature type
	CBSignType signtype;
	if (strcmp(signTypeString, "CB_SIGHASH_ALL") == 0) {
		signtype = CB_SIGHASH_ALL;
	}
	else if(strcmp(signTypeString, "CB_SIGHASH_NONE") == 0){
		signtype = CB_SIGHASH_NONE;
	}
	else if(strcmp(signTypeString, "CB_SIGHASH_SINGLE") == 0){
		signtype = CB_SIGHASH_SINGLE;
	}
	else if(strcmp(signTypeString, "CB_SIGHASH_ANYONECANPAY") == 0){
		signtype = CB_SIGHASH_ANYONECANPAY;
	}
	else{
		// we have to fail here
		return "NULL";
	}
        //CBLogError("sign 2.(%d)",input);
/*
	CBTransactionSignPubKeyHashInput(
		tx
		,keypair->keyPair
		, prevOutSubScript
		, (uint32_t)input
		, CB_SIGHASH_ALL
	);*/
	CBScript * oldprevOutSubScript = tx->inputs[input]->scriptObject;
        //CBLogError("sign 3.");
	if (!CBTransactionSignPubKeyHashInput(tx, keypair->keyPair,
			oldprevOutSubScript, input, signtype)){
		CBLogError("Unable to add a signature to a pubkey hash transaction input.");
		return "NULL";
	}
/*
	tx->inputs[input]->scriptObject = CBNewScriptOfSize(CB_PUBKEY_SIZE + CB_MAX_DER_SIG_SIZE + 3);
	uint8_t sigLen = CBTransactionAddSignature(tx, tx->inputs[input]->scriptObject, 0,
			keypair->keyPair, oldprevOutSubScript, input, signtype);
	if (!sigLen){
		CBLogError("Unable to add a signature to a pubkey hash transaction input.");
		return "NULL";
	}

	// add the public key
	CBByteArraySetByte(tx->inputs[input]->scriptObject, sigLen, CB_PUBKEY_SIZE);
	memcpy(CBByteArrayGetData(tx->inputs[input]->scriptObject) + sigLen + 1, keypair->keyPair->pubkey.key, CB_PUBKEY_SIZE);
	//return txString;
*/
	return obj_to_serializeddata(tx);

}
bool CBTransactionSignMultisigInputV2(CBTransaction * self, CBKeyPair * key, CBByteArray * prevOutSubScript, uint32_t input, CBSignType signType) {
	CBScript * inScript;
	uint16_t offset;
	if (self->inputs[input]->scriptObject) {
		offset = self->inputs[input]->scriptObject->length;
		inScript = CBNewByteArrayOfSize(offset + CB_MAX_DER_SIG_SIZE + 2);
		CBByteArrayCopyByteArray(inScript, 0, self->inputs[input]->scriptObject);
		CBReleaseObject(self->inputs[input]->scriptObject);
		self->inputs[input]->scriptObject = inScript;
	}else{
		inScript = self->inputs[input]->scriptObject = CBNewScriptOfSize(CB_MAX_DER_SIG_SIZE + 3);
		CBByteArraySetByte(inScript, 0, CB_SCRIPT_OP_0);
		offset = 1;
	}
	return CBTransactionAddSignature(self, inScript, offset, key, prevOutSubScript, input, signType);
}

char* sign_tx_multisig(char* txString, char* keypairString, char* prevOutSubScriptString, int input, char* signTypeString){
	CBTransaction * tx = serializeddata_to_obj(txString);
	CBHDKey * keypair = cbhdkey_serializeddata_to_obj(keypairString);
	//CBScript * prevOutSubScript = script_serializeddata_to_obj(prevOutSubScriptString);
	CBScript * prevOutSubScript = CBByteArrayCopy((CBByteArray*) tx->inputs[input]->scriptObject);


	// figure out the signature type
	CBSignType signtype;
	if (strcmp(signTypeString, "CB_SIGHASH_ALL") == 0) {
		signtype = CB_SIGHASH_ALL;
	}
	else if(strcmp(signTypeString, "CB_SIGHASH_NONE") == 0){
		signtype = CB_SIGHASH_NONE;
	}
	else if(strcmp(signTypeString, "CB_SIGHASH_SINGLE") == 0){
		signtype = CB_SIGHASH_SINGLE;
	}
	else if(strcmp(signTypeString, "CB_SIGHASH_ANYONECANPAY") == 0){
		signtype = CB_SIGHASH_ANYONECANPAY;
	}
	else{
		// we have to fail here
		return "NULL";
	}

	//CBScript * oldprevOutSubScript = tx->inputs[input]->scriptObject;
	/*
	 * CBTransactionSignMultisigInput(
			CBTransaction * self, CBKeyPair * key, CBByteArray * prevOutSubScript, uint32_t input, CBSignType signType
		)
	 */
	if (!CBTransactionSignMultisigInputV2(tx, keypair->keyPair, prevOutSubScript, input, signtype)){
		CBLogError("Unable to add a signature to a pubkey hash transaction input.");
		return "NULL";
	}

	return obj_to_serializeddata(tx);
}


#line 377 "Transaction.c"
#ifndef PERL_UNUSED_VAR
#  define PERL_UNUSED_VAR(var) if (0) var = var
#endif

#ifndef PERL_ARGS_ASSERT_CROAK_XS_USAGE
#define PERL_ARGS_ASSERT_CROAK_XS_USAGE assert(cv); assert(params)

/* prototype to pass -Wmissing-prototypes */
STATIC void
S_croak_xs_usage(pTHX_ const CV *const cv, const char *const params);

STATIC void
S_croak_xs_usage(pTHX_ const CV *const cv, const char *const params)
{
    const GV *const gv = CvGV(cv);

    PERL_ARGS_ASSERT_CROAK_XS_USAGE;

    if (gv) {
        const char *const gvname = GvNAME(gv);
        const HV *const stash = GvSTASH(gv);
        const char *const hvname = stash ? HvNAME(stash) : NULL;

        if (hvname)
            Perl_croak(aTHX_ "Usage: %s::%s(%s)", hvname, gvname, params);
        else
            Perl_croak(aTHX_ "Usage: %s(%s)", gvname, params);
    } else {
        /* Pants. I don't think that it should be possible to get here. */
        Perl_croak(aTHX_ "Usage: CODE(0x%"UVxf")(%s)", PTR2UV(cv), params);
    }
}
#undef  PERL_ARGS_ASSERT_CROAK_XS_USAGE

#ifdef PERL_IMPLICIT_CONTEXT
#define croak_xs_usage(a,b)	S_croak_xs_usage(aTHX_ a,b)
#else
#define croak_xs_usage		S_croak_xs_usage
#endif

#endif

/* NOTE: the prototype of newXSproto() is different in versions of perls,
 * so we define a portable version of newXSproto()
 */
#ifdef newXS_flags
#define newXSproto_portable(name, c_impl, file, proto) newXS_flags(name, c_impl, file, proto, 0)
#else
#define newXSproto_portable(name, c_impl, file, proto) (PL_Sv=(SV*)newXS(name, c_impl, file), sv_setpv(PL_Sv, proto), (CV*)PL_Sv)
#endif /* !defined(newXS_flags) */

#line 429 "Transaction.c"

XS(XS_CBitcoin__Transaction_create_tx_obj); /* prototype to pass -Wmissing-prototypes */
XS(XS_CBitcoin__Transaction_create_tx_obj)
{
#ifdef dVAR
    dVAR; dXSARGS;
#else
    dXSARGS;
#endif
    if (items != 6)
       croak_xs_usage(cv,  "lockTime, version, inputs, outputs, numOfInputs, numOfOutputs");
    {
	int	lockTime = (int)SvIV(ST(0));
	int	version = (int)SvIV(ST(1));
	SV *	inputs = ST(2);
	SV *	outputs = ST(3);
	int	numOfInputs = (int)SvIV(ST(4));
	int	numOfOutputs = (int)SvIV(ST(5));
	char *	RETVAL;
	dXSTARG;

	RETVAL = create_tx_obj(lockTime, version, inputs, outputs, numOfInputs, numOfOutputs);
	sv_setpv(TARG, RETVAL); XSprePUSH; PUSHTARG;
    }
    XSRETURN(1);
}


XS(XS_CBitcoin__Transaction_get_numOfInputs); /* prototype to pass -Wmissing-prototypes */
XS(XS_CBitcoin__Transaction_get_numOfInputs)
{
#ifdef dVAR
    dVAR; dXSARGS;
#else
    dXSARGS;
#endif
    if (items != 1)
       croak_xs_usage(cv,  "serializedDataString");
    {
	char *	serializedDataString = (char *)SvPV_nolen(ST(0));
	int	RETVAL;
	dXSTARG;

	RETVAL = get_numOfInputs(serializedDataString);
	XSprePUSH; PUSHi((IV)RETVAL);
    }
    XSRETURN(1);
}


XS(XS_CBitcoin__Transaction_get_numOfOutputs); /* prototype to pass -Wmissing-prototypes */
XS(XS_CBitcoin__Transaction_get_numOfOutputs)
{
#ifdef dVAR
    dVAR; dXSARGS;
#else
    dXSARGS;
#endif
    if (items != 1)
       croak_xs_usage(cv,  "serializedDataString");
    {
	char *	serializedDataString = (char *)SvPV_nolen(ST(0));
	int	RETVAL;
	dXSTARG;

	RETVAL = get_numOfOutputs(serializedDataString);
	XSprePUSH; PUSHi((IV)RETVAL);
    }
    XSRETURN(1);
}


XS(XS_CBitcoin__Transaction_get_Input); /* prototype to pass -Wmissing-prototypes */
XS(XS_CBitcoin__Transaction_get_Input)
{
#ifdef dVAR
    dVAR; dXSARGS;
#else
    dXSARGS;
#endif
    if (items != 2)
       croak_xs_usage(cv,  "serializedDataString, InputIndex");
    {
	char *	serializedDataString = (char *)SvPV_nolen(ST(0));
	int	InputIndex = (int)SvIV(ST(1));
	char *	RETVAL;
	dXSTARG;

	RETVAL = get_Input(serializedDataString, InputIndex);
	sv_setpv(TARG, RETVAL); XSprePUSH; PUSHTARG;
    }
    XSRETURN(1);
}


XS(XS_CBitcoin__Transaction_get_Output); /* prototype to pass -Wmissing-prototypes */
XS(XS_CBitcoin__Transaction_get_Output)
{
#ifdef dVAR
    dVAR; dXSARGS;
#else
    dXSARGS;
#endif
    if (items != 2)
       croak_xs_usage(cv,  "serializedDataString, OutputIndex");
    {
	char *	serializedDataString = (char *)SvPV_nolen(ST(0));
	int	OutputIndex = (int)SvIV(ST(1));
	char *	RETVAL;
	dXSTARG;

	RETVAL = get_Output(serializedDataString, OutputIndex);
	sv_setpv(TARG, RETVAL); XSprePUSH; PUSHTARG;
    }
    XSRETURN(1);
}


XS(XS_CBitcoin__Transaction_hash_of_tx); /* prototype to pass -Wmissing-prototypes */
XS(XS_CBitcoin__Transaction_hash_of_tx)
{
#ifdef dVAR
    dVAR; dXSARGS;
#else
    dXSARGS;
#endif
    if (items != 1)
       croak_xs_usage(cv,  "serializedDataString");
    {
	char *	serializedDataString = (char *)SvPV_nolen(ST(0));
	char *	RETVAL;
	dXSTARG;

	RETVAL = hash_of_tx(serializedDataString);
	sv_setpv(TARG, RETVAL); XSprePUSH; PUSHTARG;
    }
    XSRETURN(1);
}


XS(XS_CBitcoin__Transaction_get_lockTime_from_obj); /* prototype to pass -Wmissing-prototypes */
XS(XS_CBitcoin__Transaction_get_lockTime_from_obj)
{
#ifdef dVAR
    dVAR; dXSARGS;
#else
    dXSARGS;
#endif
    if (items != 1)
       croak_xs_usage(cv,  "serializedDataString");
    {
	char *	serializedDataString = (char *)SvPV_nolen(ST(0));
	int	RETVAL;
	dXSTARG;

	RETVAL = get_lockTime_from_obj(serializedDataString);
	XSprePUSH; PUSHi((IV)RETVAL);
    }
    XSRETURN(1);
}


XS(XS_CBitcoin__Transaction_get_version_from_obj); /* prototype to pass -Wmissing-prototypes */
XS(XS_CBitcoin__Transaction_get_version_from_obj)
{
#ifdef dVAR
    dVAR; dXSARGS;
#else
    dXSARGS;
#endif
    if (items != 1)
       croak_xs_usage(cv,  "serializedDataString");
    {
	char *	serializedDataString = (char *)SvPV_nolen(ST(0));
	int	RETVAL;
	dXSTARG;

	RETVAL = get_version_from_obj(serializedDataString);
	XSprePUSH; PUSHi((IV)RETVAL);
    }
    XSRETURN(1);
}


XS(XS_CBitcoin__Transaction_sign_tx_pubkeyhash); /* prototype to pass -Wmissing-prototypes */
XS(XS_CBitcoin__Transaction_sign_tx_pubkeyhash)
{
#ifdef dVAR
    dVAR; dXSARGS;
#else
    dXSARGS;
#endif
    if (items != 5)
       croak_xs_usage(cv,  "txString, keypairString, prevOutSubScriptString, input, signTypeString");
    {
	char *	txString = (char *)SvPV_nolen(ST(0));
	char *	keypairString = (char *)SvPV_nolen(ST(1));
	char *	prevOutSubScriptString = (char *)SvPV_nolen(ST(2));
	int	input = (int)SvIV(ST(3));
	char *	signTypeString = (char *)SvPV_nolen(ST(4));
	char *	RETVAL;
	dXSTARG;

	RETVAL = sign_tx_pubkeyhash(txString, keypairString, prevOutSubScriptString, input, signTypeString);
	sv_setpv(TARG, RETVAL); XSprePUSH; PUSHTARG;
    }
    XSRETURN(1);
}


XS(XS_CBitcoin__Transaction_sign_tx_multisig); /* prototype to pass -Wmissing-prototypes */
XS(XS_CBitcoin__Transaction_sign_tx_multisig)
{
#ifdef dVAR
    dVAR; dXSARGS;
#else
    dXSARGS;
#endif
    if (items != 5)
       croak_xs_usage(cv,  "txString, keypairString, prevOutSubScriptString, input, signTypeString");
    {
	char *	txString = (char *)SvPV_nolen(ST(0));
	char *	keypairString = (char *)SvPV_nolen(ST(1));
	char *	prevOutSubScriptString = (char *)SvPV_nolen(ST(2));
	int	input = (int)SvIV(ST(3));
	char *	signTypeString = (char *)SvPV_nolen(ST(4));
	char *	RETVAL;
	dXSTARG;

	RETVAL = sign_tx_multisig(txString, keypairString, prevOutSubScriptString, input, signTypeString);
	sv_setpv(TARG, RETVAL); XSprePUSH; PUSHTARG;
    }
    XSRETURN(1);
}

#ifdef __cplusplus
extern "C"
#endif
XS(boot_CBitcoin__Transaction); /* prototype to pass -Wmissing-prototypes */
XS(boot_CBitcoin__Transaction)
{
#ifdef dVAR
    dVAR; dXSARGS;
#else
    dXSARGS;
#endif
#if (PERL_REVISION == 5 && PERL_VERSION < 9)
    char* file = __FILE__;
#else
    const char* file = __FILE__;
#endif

    PERL_UNUSED_VAR(cv); /* -W */
    PERL_UNUSED_VAR(items); /* -W */
#ifdef XS_APIVERSION_BOOTCHECK
    XS_APIVERSION_BOOTCHECK;
#endif
    XS_VERSION_BOOTCHECK ;

        newXS("CBitcoin::Transaction::create_tx_obj", XS_CBitcoin__Transaction_create_tx_obj, file);
        newXS("CBitcoin::Transaction::get_numOfInputs", XS_CBitcoin__Transaction_get_numOfInputs, file);
        newXS("CBitcoin::Transaction::get_numOfOutputs", XS_CBitcoin__Transaction_get_numOfOutputs, file);
        newXS("CBitcoin::Transaction::get_Input", XS_CBitcoin__Transaction_get_Input, file);
        newXS("CBitcoin::Transaction::get_Output", XS_CBitcoin__Transaction_get_Output, file);
        newXS("CBitcoin::Transaction::hash_of_tx", XS_CBitcoin__Transaction_hash_of_tx, file);
        newXS("CBitcoin::Transaction::get_lockTime_from_obj", XS_CBitcoin__Transaction_get_lockTime_from_obj, file);
        newXS("CBitcoin::Transaction::get_version_from_obj", XS_CBitcoin__Transaction_get_version_from_obj, file);
        newXS("CBitcoin::Transaction::sign_tx_pubkeyhash", XS_CBitcoin__Transaction_sign_tx_pubkeyhash, file);
        newXS("CBitcoin::Transaction::sign_tx_multisig", XS_CBitcoin__Transaction_sign_tx_multisig, file);
#if (PERL_REVISION == 5 && PERL_VERSION >= 9)
  if (PL_unitcheckav)
       call_list(PL_scopestack_ix, PL_unitcheckav);
#endif
    XSRETURN_YES;
}

