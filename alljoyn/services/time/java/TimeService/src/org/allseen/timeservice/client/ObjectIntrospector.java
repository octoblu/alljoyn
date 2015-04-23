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

package org.allseen.timeservice.client;

import java.util.Arrays;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

import org.alljoyn.bus.BusException;
import org.alljoyn.bus.ProxyBusObject;
import org.alljoyn.bus.ifaces.AllSeenIntrospectable;
import org.allseen.timeservice.TimeServiceException;

import android.util.Log;

/**
 * This class introspects the object identified by the Object Path provided in the class constructor.
 * The introspection is done with AllJoyn {@link AllSeenIntrospectable} interface.
 */
abstract class ObjectIntrospector extends TimeClientBase {
    private static final String TAG = "ajts" + ObjectIntrospector.class.getSimpleName();

    /**
     * Maps language to description that was found in the introspected object
     */
    private final Map<String, String> objectDescriptions;

    /**
     * Description languages retrieved by the {@link AllSeenIntrospectable#GetDescriptionLanguages()}
     */
    private String[] descriptionLanguages;

    /**
     * The string to be used for {@link AllSeenIntrospectable#IntrospectWithDescription(String)}
     * when no language is specified
     */
    private static final String NO_LANG  = "";

    /**
     * Constructor
     * @param tsClient {@link TimeServiceClient}
     * @param objectPath Object Path of the introspected object
     */
    ObjectIntrospector(TimeServiceClient tsClient, String objectPath) {

        super(tsClient, objectPath);
        objectDescriptions = new ConcurrentHashMap<String, String>();
    }

    /**
     * Retrieves array of description languages supported by the introspected object.
     * @return Array of description languages or NULL if failed to retrieve the description languages.
     * If the returned array is empty, it means that the introspected object has no description.
     * @see AllSeenIntrospectable#GetDescriptionLanguages()
     */
    public String[] retrieveDescriptionLanguages() {

        if ( descriptionLanguages != null ) {

            return descriptionLanguages;
        }

        //Retrieve description languages
        Log.d(TAG, "Retrieving description languages, object: '" + objectPath + "'");

        ProxyBusObject proxyObj;

        try {

             proxyObj = getProxyObject(new Class<?>[]{AllSeenIntrospectable.class});
        } catch (TimeServiceException tse) {

            Log.e(TAG, "Failed to create proxy object to retrieve description languages, object: '" + objectPath + "'", tse);
            return descriptionLanguages;
        }

        AllSeenIntrospectable introspector = proxyObj.getInterface(AllSeenIntrospectable.class);
        try {

            descriptionLanguages = introspector.GetDescriptionLanguages();
        } catch (BusException be) {

            Log.e(TAG, "Failed to retrieve description languages, object: '" + objectPath + "'", be);
            return descriptionLanguages;
        }

        Log.d(TAG, "Returning description languages: '" + Arrays.toString(descriptionLanguages) + "', object: '" +
                 objectPath + "'");

        return descriptionLanguages;
    }

    /**
     * Retrieve description of the introspected object. <br>
     * The description is retrieved in the requested language. The language should be
     * one of those returned by the {@link ObjectIntrospector#retrieveDescriptionLanguages()}.
     * @param language Language of the retrieved object description
     * @return Object description or empty string if no description was found
     * @throws IllegalArgumentException Is thrown if received language is not valid, which means it is NULL
     * or was not returned by the {@link ObjectIntrospector#retrieveDescriptionLanguages()}
     */
    public String retrieveObjectDescription(String language) {

        checkDescLanguageValidity(language);

        //No description
        if ( NO_LANG.equals(language) ) {

            return "";
        }

        String description = objectDescriptions.get(language);

        //There is cached description for the requested language
        if (  description != null ) {

            return description;
        }

        Log.d(TAG, "Retrieving description for the language: '" + language + "', objPath: '" + objectPath + "'");

        //No description for the requested language
        IntrospectionNode introspector;

        try {

            introspector = introspect(language);
        } catch (Exception e) {

            Log.e(TAG, "Failed to introspect the object: '" + objectPath + "'", e);
            return "";
        }

        description = introspector.getObjectDescription();

        objectDescriptions.put(language, description);

        Log.d(TAG, "Returned Object Description: '" + description + "' language: '" + language + "', object: '" + objectPath + "'");
        return description;
    }

    /**
     * Introspects without description the object identified by the Object Path
     * @return {@link IntrospectionNode}
     * @throws Exception Is thrown if failed to introspect the object
     */
    IntrospectionNode introspect() throws Exception {

        return introspect(NO_LANG);
    }

    /**
     * Introspects with description the object identified by the Object Path
     * @return {@link IntrospectionNode}
     * @throws Exception Is thrown if failed to introspect the object
     * @see AllSeenIntrospectable
     */
    IntrospectionNode introspect(String language) throws Exception {

        checkTsClientValidity();

        Log.d(TAG, "Instrospecting object: '" + objectPath + "', description language: '" + language + "'");
        IntrospectionNode introspector = new IntrospectionNode(objectPath);
        introspector.parse(getBus(), tsClient.getServerBusName(), getSid(), language);

        return introspector;
    }

    /**
     * Set the object description for the given language
     * @param language The language of the object description
     * @param objectDescription objectDescription
     */
    void setObjectDescription(String language, String objectDescription) {

        checkDescLanguageValidity(language);

        //Can't be description of NO_LANG
        if ( NO_LANG.equals(language) ) {

            return;
        }

        objectDescriptions.put(language, objectDescription);
    }

    /**
     * @see org.allseen.timeservice.client.TimeClientBase#release()
     */
    @Override
    public void release() {

        descriptionLanguages = null;
        objectDescriptions.clear();

        super.release();
    }

    /**
     * Checks validity of the given descriptionLanguage
     * @param descriptionLanguage Language to be checked
     * @throws IllegalArgumentException Is thrown if the language is undefined, not {@link ObjectIntrospector#NO_LANG}
     * or not one of the descriptionLanguages array
     */
    void checkDescLanguageValidity(String descriptionLanguage) {

        if ( descriptionLanguage == null ) {

            throw new IllegalArgumentException("Undefined descriptionLanguage");
        }

        //If language is not NO_LANG and not in description languages array
        if ( !descriptionLanguage.equals(NO_LANG) &&
                !inArray(retrieveDescriptionLanguages(), descriptionLanguage) ) {

            throw new IllegalArgumentException("Not valid description language");
        }
    }

    /**
     * Search for the given language in the languages array
     * @param languages Search for the language in this array of languages
     * @param language Language to search in the array
     * @return TRUE of the language was found
     */
    private boolean inArray(String[] languages, String language) {

        if ( languages == null || languages.length == 0 ) {

            return false;
        }

        for (String cl : languages ) {

            if ( cl.equals(language) ) {

                return true;
            }
        }

        return false;
    }
}
