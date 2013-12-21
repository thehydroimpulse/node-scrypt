/*
scrypt_common.cc

Copyright (C) 2013 Barry Steyn (http://doctrina.org/Scrypt-Authentication-For-Node.html)

This source code is provided 'as-is', without any express or implied
warranty. In no event will the author be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this source code must not be misrepresented; you must not
claim that you wrote the original source code. If you use this source code
in a product, an acknowledgment in the product documentation would be
appreciated but is not required.

2. Altered source versions must be plainly marked as such, and must not be
misrepresented as being the original source code.

3. This notice may not be removed or altered from any source distribution.

Barry Steyn barry.steyn@gmail.com

*/


//
// Common functionality needed by boiler plate code
//

#include <node.h>
#include <v8.h>
#include <string>
#include "scrypt_common.h"

namespace Internal {
	//
	// Error codes
	//

	namespace {
	//
	// Returns error descriptions as generated by Scrypt
	//
	std::string ScryptErrorDescr(const int error) {
		switch(error) {
			case 0: 
				return "success";
			case 1: 
				return "getrlimit or sysctl(hw.usermem) failed";
			case 2: 
				return "clock_getres or clock_gettime failed";
			case 3: 
				return "error computing derived key";
			case 4: 
				return "could not read salt from /dev/urandom";
			case 5: 
				return "error in OpenSSL";
			case 6: 
				return "malloc failed";
			case 7: 
				return "data is not a valid scrypt-encrypted block";
			case 8: 
				return "unrecognized scrypt format";
			case 9:     
				return "decrypting file would take too much memory";
			case 10: 
				return "decrypting file would take too long";
			case 11: 
				return "password is incorrect";
			case 12: 
				return "error writing output file";
			case 13: 
				return "error reading input file";
			default:
				return "error unkown";
		}
	}

	} //end anon namespace

	//
	// Checks that ScryptParams object is "Kosher"
	//
	int CheckScryptParameters(const v8::Local<v8::Object> &obj, std::string& errMessage) {
		v8::Local<v8::Value> val;
		if (!obj->Has(v8::String::New("N"))) {
			errMessage = "N value is not present";
			return 1;
		}

		if (!obj->Has(v8::String::New("r"))) {
			errMessage = "r value is not present";
			return 1;
		}

		if (!obj->Has(v8::String::New("p"))) {
			errMessage = "p value is not present";
			return 1;
		}

		val = obj->Get(v8::String::New("N"));
		if (!val->IsNumber()) {
			errMessage = "N must be a numeric value";
		}
		
		val = obj->Get(v8::String::New("r"));
		if (!val->IsNumber()) {
			errMessage = "r must be a numeric value";
		}
		
		val = obj->Get(v8::String::New("p"));
		if (!val->IsNumber()) {
			errMessage = "p must be a numeric value";
		}
		
		return 0;
	}

	//
	// Definition for assignment operator
	//
	void ScryptParams::operator=(const v8::Local<v8::Object> &rhs) {
		this->N = rhs->Get(v8::String::New("N"))->ToNumber()->Value();
		this->r = rhs->Get(v8::String::New("r"))->ToNumber()->Value();
		this->p = rhs->Get(v8::String::New("p"))->ToNumber()->Value();
	}

	//
	// Produces a JSON error object
	//
	v8::Local<v8::Value> MakeErrorObject(int errCode, const char* errMessage) {
		v8::Local<v8::Object> obj = v8::Object::New();

		if (errCode == INTERNARG || errCode == JSARG) {
			obj->Set(v8::String::NewSymbol("err_code"), v8::Integer::New(errCode));
			obj->Set(v8::String::NewSymbol("err_message"), v8::String::New(errMessage));
		} else {
			obj->Set(v8::String::NewSymbol("err_code"), v8::Integer::New(500));
			if (errMessage)
				obj->Set(v8::String::NewSymbol("internal_message"), v8::String::New(errMessage));
			obj->Set(v8::String::NewSymbol("err_message"), v8::String::New("Unknown internal error - please report this error to make this module better. Details about reporting the error can be found at..."));
		}

		return obj;
	}

	//
	// Produces a JSON error object for errors resulting from Scrypt
	//
	v8::Local<v8::Value> MakeErrorObject(int errCode, int scryptErrorCode) {
		assert(scryptErrorCode == SCRYPT);
		v8::Local<v8::Object> obj = v8::Object::New();
		obj->Set(v8::String::NewSymbol("err_code"), v8::Integer::New(errCode));
		obj->Set(v8::String::NewSymbol("err_message"), v8::String::New("Scrypt internal error"));
		obj->Set(v8::String::NewSymbol("scrypt_err_code"),v8::Integer::New(scryptErrorCode));
		obj->Set(v8::String::NewSymbol("scrypt_err_message"),v8::String::New(ScryptErrorDescr(scryptErrorCode).c_str()));
		return obj;
	}
} //end internal namespace
