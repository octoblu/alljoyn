 /******************************************************************************
  * Copyright AllSeen Alliance. All rights reserved.
  *
  *    Permission to use, copy, modify, and/or distribute this software for any
  *    purpose with or without fee is hereby granted, provided that the above
  *    copyright notice and this permission notice appear in all copies.
  *
  *    THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
  *    WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
  *    MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
  *    ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
  *    WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
  *    ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
  *    OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
  ******************************************************************************/

package org.alljoyn.ioe.controlpanelservice.ui;

/**
 * Possible types of list of properties UI hints 
 */
public enum ListPropertyWidgetHintsType {
	DYNAMIC_SPINNER((short)1);
	
    /** 
     * The key number
     */  
    public final short ID; 
    
    /** 
     * Constructor
     * @param id
     */
    private ListPropertyWidgetHintsType(short id) {
        ID = id; 
    }   

    /**
     * Search for the enum by the given id
     * If not found returns NULL
     * @param id  
     * @return Enum type by the given id
     */
    public static ListPropertyWidgetHintsType getEnumById(short id) {
    	ListPropertyWidgetHintsType retType = null;
        for (ListPropertyWidgetHintsType type : ListPropertyWidgetHintsType.values()) {
            if ( id == type.ID ) {
                retType = type;
                break;
            }
        }
        return retType;
    }//getEnumById
}
