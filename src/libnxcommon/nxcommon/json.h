/*
	Copyright 2010-2013 David "Alemarius Nexus" Lerch

	This file is part of nxcommon.

	nxcommon is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	nxcommon is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with nxcommon.  If not, see <http://www.gnu.org/licenses/>.

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#ifndef NXCOMMON_SRC_LIBNXCOMMON_NXCOMMON_JSON_H_
#define NXCOMMON_SRC_LIBNXCOMMON_NXCOMMON_JSON_H_

#include <rapidjson/document.h>
#include "CString.h"


template <typename GenericValue>
void CascadeJSON(GenericValue& base, GenericValue& ovr, typename GenericValue::AllocatorType& alloc)
{
	using namespace rapidjson;

	switch (ovr.GetType()) {
	/*case kNullType:
		base.SetNull();
		break;

    case kFalseType:
    	base.SetBool(false);
    	break;

    case kTrueType:
    	base.SetBool(true);
    	break;*/

    case kObjectType:
    {
    	if (!base.IsObject()) {
    		base.SetObject();
    	}

    	for (auto it = ovr.MemberBegin() ; it != ovr.MemberEnd() ; it++) {
    		CString ovrChildName = CString::readAlias(it->name.GetString(), it->name.GetStringLength());
    		Value& ovrChild = it->value;

    		if (ovrChildName.endsWith("__append")) {
    			CString baseName = ovrChildName.substr(0, ovrChildName.length() - strlen("__append"));

    			if (base.HasMember(baseName.get())) {
    				// Base already has this member -> an actual append

    				Value& baseChild = base[baseName.get()];

    				if (baseChild.IsArray()) {
    					// Array append

    					if (ovrChild.IsArray()) {
    						// Append all values in the array

    						for (SizeType i = 0 ; i < ovrChild.Size() ; i++) {
								baseChild.PushBack(Value(), alloc);
								CascadeJSON(baseChild[baseChild.Size() - 1], ovrChild[i], alloc);
    						}
    					} else {
    						// Append single value to array

    						baseChild.PushBack(Value(), alloc);
    						CascadeJSON(baseChild[baseChild.Size() - 1], ovrChild, alloc);
    					}
    				} else if (baseChild.IsObject()) {
    					// This is just a plain object cascade (no special append semantics)

    					CascadeJSON(baseChild, ovrChild, alloc);
    				} else if (baseChild.IsString()) {
    					// String append

    					if (ovrChild.IsString()) {
    						CString combined(baseChild.GetString(), baseChild.GetStringLength());
    						combined.append(CString::readAlias(ovrChild.GetString(), ovrChild.GetStringLength()));
    						baseChild.SetString(combined.get(), (SizeType) combined.length(), alloc);
    					} else {
    						// TODO: Report somehow
    						//LogError("Invalid type for config cascading append. Override option should be a string.");
    					}
    				} else {
    					// TODO: Report somehow
    					//LogError("Invalid base datatype for config cascading append.");
    				}
    			} else {
    				// Member does not exist in base -> this is a simple set operation

    				base.AddMember(Value(baseName.get(), (SizeType) baseName.length(), alloc), Value(), alloc);
    				CascadeJSON(base[baseName.get()], ovrChild, alloc);
    			}
    		} else if (ovrChildName.endsWith("__remove")) {
    			CString baseName = ovrChildName.substr(0, (SizeType) ovrChildName.length() - strlen("__remove"));
    			base.EraseMember(baseName.get());
    		} else {
    			if (base.HasMember(ovrChildName.get())) {
    				// An existing member is modified
    				CascadeJSON(base[ovrChildName.get()], ovrChild, alloc);
    			} else {
    				// A new member was added. We need to cascade further instead of copying, because
    				// we have to resolve __append and __remove elements
    				base.AddMember(Value(ovrChildName.get(), (SizeType) ovrChildName.length(), alloc), Value(), alloc);
    				CascadeJSON(base[ovrChildName.get()], ovrChild, alloc);
    			}
    		}
		}

    	break;
    }

    default:
    	base.CopyFrom(Value(ovr, alloc), alloc);
    	break;

    /*case kArrayType:
    	// This is an overwrite. Appending is handled by kObjectType above
    	base.CopyFrom(Value(ovr, alloc), alloc);
    	break;

    case kStringType:
    	// This is an overwrite. Appending is handled by kObjectType above
    	base.SetString(ovr.GetString(), ovr.GetStringLength(), alloc);
    	break;

    case kNumberType:
    	base.CopyFrom(Value(ovr, alloc), alloc);
    	break;

    default:
    	assert(false);*/
	}
}

#endif /* NXCOMMON_SRC_LIBNXCOMMON_NXCOMMON_JSON_H_ */
