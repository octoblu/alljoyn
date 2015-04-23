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

package org.alljoyn.services.android.storage;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.Map;
import java.util.Set;
import java.util.UUID;

import javax.xml.parsers.ParserConfigurationException;

import org.alljoyn.about.AboutKeys;
import org.alljoyn.bus.AboutDataListener;
import org.alljoyn.bus.ErrorReplyBusException;
import org.alljoyn.bus.Variant;
import org.alljoyn.services.common.PropertyStore;
import org.alljoyn.services.common.PropertyStoreException;
import org.alljoyn.services.common.utils.TransportUtil;
import org.xml.sax.SAXException;

import android.content.Context;
import android.content.res.AssetManager;
import android.util.Log;

/**
 * A default implementation of the PropertyStore. Reads the factory defaults
 * from assets/Config.xml. Stores custom configuration in local XML files.
 */
public class PropertyStoreImpl implements PropertyStore, AboutDataListener
{
    private static final String CONFIG_XML = "Config.xml";
    public static final String TAG = PropertyStoreImpl.class.getName();
    private String m_defaultLanguage = "en";
    private Set<String> m_supportedLanguages = new HashSet<String>();

    /**
     * configuration storage.
     */
    private final AssetManager m_assetMgr;
    private Context m_context;
    private Map<String, Property> m_aboutConfigMap;

    public PropertyStoreImpl(Context context)
    {
        super();
        this.m_context = context;
        m_assetMgr = context.getAssets();
        m_context = context;

        // read factory defaults from assets/Config.xml
        loadFactoryDefaults();

        // figure out the available languages from the contents of Config.xml
        loadLanguages();

        // read modified-and-persisted configuration and override factory
        // defaults
        loadStoredConfiguration();

        // read the user's default language
        setDefaultLanguageFromProperties();

        // load the unique Id. Create if not exist
        loadAppId();

        // load the device Id. Create if not exist
        loadDeviceId();

        storeConfiguration();
    }

    private void getConfiguration(String languageTag, Map<String, Object> configuration)
    {
        for (String key : m_aboutConfigMap.keySet())
        {
            Property property = m_aboutConfigMap.get(key);
            if (!property.isPublic() || !property.isWritable())
            {
                continue;
            }
            Object value = property.getValue(languageTag, m_defaultLanguage);
            if (value != null)
                configuration.put(key, value);
        }
    }

    private void getAbout(String languageTag, Map<String, Object> about)
    {
        for (String key : m_aboutConfigMap.keySet())
        {
            Property property = m_aboutConfigMap.get(key);
            if (!property.isPublic())
            {
                continue;
            }
            Object value = property.getValue(languageTag, m_defaultLanguage);
            if (value != null)
                about.put(key, value);
        }
    }

    private void getAnnouncement(String languageTag, Map<String, Object> announce)
    {
        for (String key : m_aboutConfigMap.keySet())
        {
            Property property = m_aboutConfigMap.get(key);
            if (!property.isPublic() || !property.isAnnounced())
            {
                continue;
            }
            Object value = property.getValue(languageTag, m_defaultLanguage);
            if (value != null)
                announce.put(key, value);
        }
    }

    /**
     * Set a value for a property
     * 
     * @param key
     *            property name
     * @param value
     *            property value
     * @param languageTag
     *            the language for which to set the value
     * @see Property
     */
    public void setValue(String key, Object value, String languageTag)
    {
        Property property = m_aboutConfigMap.get(key);
        if (property == null)
        {
            property = new Property(key, true, true, true);
            m_aboutConfigMap.put(key, property);
        }
        property.setValue(languageTag, value);

    }

    private void loadLanguages()
    {
        Set<String> languages = new HashSet<String>(3);
        for (String key : m_aboutConfigMap.keySet())
        {
            Property property = m_aboutConfigMap.get(key);
            Set<String> langs = property.getLanguages();
            if (langs.size() != 0)
                languages.addAll(langs);
        }
        languages.remove(Property.NO_LANGUAGE);
        m_supportedLanguages = languages;
        Property property = new Property(AboutKeys.ABOUT_SUPPORTED_LANGUAGES, false, false, true);
        property.setValue(Property.NO_LANGUAGE, m_supportedLanguages);
        m_aboutConfigMap.put(AboutKeys.ABOUT_SUPPORTED_LANGUAGES, property);
    }

    private void loadFactoryDefaults()
    {
        try
        {
            InputStream is = m_assetMgr.open(CONFIG_XML);
            m_aboutConfigMap = PropertyParser.readFromXML(is);
        }
        catch (IOException e)
        {
            Log.e(TAG, "Error loading file assets/" + CONFIG_XML, e);
            m_aboutConfigMap = createCannedMap();
        }
        catch (ParserConfigurationException e)
        {
            Log.e(TAG, "Error parsing xml file assets/" + CONFIG_XML, e);
            m_aboutConfigMap = createCannedMap();
        }
        catch (SAXException e)
        {
            Log.e(TAG, "Error parsing xml file assets/" + CONFIG_XML, e);
            m_aboutConfigMap = createCannedMap();
        }
    }

    private void loadStoredConfiguration()
    {
        try
        {
            if (new File(m_context.getFilesDir() + "/" + CONFIG_XML).exists())
            {
                InputStream is = m_context.openFileInput(CONFIG_XML);
                Map<String, Property> storedConfiguration = PropertyParser.readFromXML(is);
                for (String key : storedConfiguration.keySet())
                {

                    Property property = m_aboutConfigMap.get(key);
                    Property storedProperty = storedConfiguration.get(key);

                    if (storedProperty != null)
                    { // should never happen

                        if (property == null)
                        {
                            m_aboutConfigMap.put(key, storedProperty);
                        }
                        else
                        {
                            for (String language : storedProperty.getLanguages())
                            {
                                Object languageValue = storedProperty.getValue(language, language);
                                property.setValue(language, languageValue);
                            }
                        }

                    }// if :: storedProperty != null

                }// for :: storedConfiguration
            }
        }
        catch (IOException e)
        {
            Log.e(TAG, "Error loading file " + CONFIG_XML, e);
        }
        catch (ParserConfigurationException e)
        {
            Log.e(TAG, "Error parsing xml file " + CONFIG_XML, e);
        }
        catch (SAXException e)
        {
            Log.e(TAG, "Error parsing xml file " + CONFIG_XML, e);
        }
    }

    private void storeConfiguration()
    {
        String localConfigFileName = CONFIG_XML;
        // Note: this one is on the app's folder, not in assets
        try
        {
            FileOutputStream openFileOutput = m_context.openFileOutput(localConfigFileName, Context.MODE_PRIVATE);
            PropertyParser.writeToXML(openFileOutput, m_aboutConfigMap);
        }
        catch (FileNotFoundException e)
        {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
        catch (IllegalArgumentException e)
        {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
        catch (IllegalStateException e)
        {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
        catch (IOException e)
        {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
    }

    private Map<String, Property> createCannedMap()
    {
        Map<String, Property> aboutMap = new HashMap<String, Property>(10);

        Property property = new Property(AboutKeys.ABOUT_APP_NAME, false, true, true);
        property.setValue(m_defaultLanguage, "Demo appname");
        aboutMap.put(AboutKeys.ABOUT_APP_NAME, property);

        property = new Property(AboutKeys.ABOUT_DEVICE_NAME, true, true, true);
        property.setValue(m_defaultLanguage, "Demo device");
        aboutMap.put(AboutKeys.ABOUT_DEVICE_NAME, property);

        property = new Property(AboutKeys.ABOUT_MANUFACTURER, false, true, true);
        property.setValue(m_defaultLanguage, "Demo manufacturer");
        aboutMap.put(AboutKeys.ABOUT_MANUFACTURER, property);

        property = new Property(AboutKeys.ABOUT_DESCRIPTION, false, false, true);
        property.setValue(m_defaultLanguage, "A default app that demonstrates the About/Config feature");
        aboutMap.put(AboutKeys.ABOUT_DESCRIPTION, property);

        property = new Property(AboutKeys.ABOUT_DEFAULT_LANGUAGE, true, true, true);
        property.setValue(m_defaultLanguage, m_defaultLanguage);
        aboutMap.put(AboutKeys.ABOUT_DEFAULT_LANGUAGE, property);

        property = new Property(AboutKeys.ABOUT_SOFTWARE_VERSION, false, false, true);
        property.setValue(m_defaultLanguage, "1.0.0.0");
        aboutMap.put(AboutKeys.ABOUT_SOFTWARE_VERSION, property);

        property = new Property(AboutKeys.ABOUT_AJ_SOFTWARE_VERSION, false, false, true);
        property.setValue(m_defaultLanguage, "3.3.1");
        aboutMap.put(AboutKeys.ABOUT_AJ_SOFTWARE_VERSION, property);

        property = new Property(AboutKeys.ABOUT_MODEL_NUMBER, false, true, true);
        property.setValue(m_defaultLanguage, "S100");
        aboutMap.put(AboutKeys.ABOUT_MODEL_NUMBER, property);

        property = new Property(AboutKeys.ABOUT_SUPPORTED_LANGUAGES, false, false, true);
        property.setValue(m_defaultLanguage, m_supportedLanguages);
        aboutMap.put(AboutKeys.ABOUT_SUPPORTED_LANGUAGES, property);

        return aboutMap;
    }

    /**
	 * 
	 */
    private void setDefaultLanguageFromProperties()
    {
        Property defaultLanguageProperty = m_aboutConfigMap.get(AboutKeys.ABOUT_DEFAULT_LANGUAGE);
        if (defaultLanguageProperty != null)
        {
            m_defaultLanguage = (String) defaultLanguageProperty.getValue(Property.NO_LANGUAGE, Property.NO_LANGUAGE);
        }
    }

    /**
     * If appId was not found in factory defaults or on persistent storage,
     * generate it
     */
    public void loadAppId()
    {
        Property appIdProperty = m_aboutConfigMap.get(AboutKeys.ABOUT_APP_ID);

        if (appIdProperty == null || appIdProperty.getValue(Property.NO_LANGUAGE, Property.NO_LANGUAGE) == null)
        {
            UUID defaultAppId = UUID.randomUUID();
            // String sAppId =
            // String.valueOf(TransportUtil.uuidToByteArray(defaultAppId));
            // here we take the stored about map, and fill gaps by default
            // values. We don't shrink the map - other existing values will
            // remain.
            Property property = new Property(AboutKeys.ABOUT_APP_ID, false, true, true);
            property.setValue(Property.NO_LANGUAGE, defaultAppId);
            m_aboutConfigMap.put(AboutKeys.ABOUT_APP_ID, property);
        }
    }

    /**
     * If uniqueId was not found in factory defaults or on persistent storage,
     * generate it
     */
    public void loadDeviceId()
    {
        Property deviceIdProperty = m_aboutConfigMap.get(AboutKeys.ABOUT_DEVICE_ID);

        if (deviceIdProperty == null || deviceIdProperty.getValue(Property.NO_LANGUAGE, Property.NO_LANGUAGE) == null)
        {
            String defaultDeviceId = String.valueOf("IoE" + System.currentTimeMillis());

            // here we take the stored about map, and fill gaps by default
            // values. We don't shrink the map - other existing values will
            // remain.
            Property property = new Property(AboutKeys.ABOUT_DEVICE_ID, false, true, true);
            property.setValue(Property.NO_LANGUAGE, defaultDeviceId);
            m_aboutConfigMap.put(AboutKeys.ABOUT_DEVICE_ID, property);
        }

    }

    /**
     * Read all the properties for a given language, filtered by a criteria
     * 
     * @param languageTag
     *            the language in which to retrieve the properties
     * @param filter
     *            filter the results by critreria: for announcement, for
     *            configuration, etc.
     * @param dataMap
     *            a map to fill with the result (to be compatible with the C++
     *            signature)
     * @throws PropertyStoreException
     *             if an unsupported language was given
     */
    @Override
    public void readAll(String languageTag, Filter filter, Map<String, Object> dataMap) throws PropertyStoreException
    {
        if (!Property.NO_LANGUAGE.equals(languageTag) && !m_supportedLanguages.contains(languageTag))
        {
            throw new PropertyStoreException(PropertyStoreException.UNSUPPORTED_LANGUAGE);
        }
        switch (filter)
        {
        case ANNOUNCE:
            getAnnouncement(languageTag, dataMap);
            break;
        case READ:
            getAbout(languageTag, dataMap);
            break;
        case WRITE:
            getConfiguration(languageTag, dataMap);
            break;
        }
    }

    /**
     * Update a property value
     * 
     * @param key
     *            the property name
     * @param languageTag
     *            the language in which the value should be updated
     * @param newValue
     *            the neew value
     * @throws PropertyStoreException
     *             for the cases: UNSUPPORTED_LANGUAGE, INVALID_VALUE,
     *             UNSUPPORTED_KEY, ILLEGAL_ACCESS
     */
    @Override
    public void update(String key, String languageTag, Object newValue) throws PropertyStoreException
    {

        Property property = m_aboutConfigMap.get(key);
        if (property == null)
        {
            throw new PropertyStoreException(PropertyStoreException.UNSUPPORTED_KEY);
        }
        if (!property.isWritable())
        {
            throw new PropertyStoreException(PropertyStoreException.ILLEGAL_ACCESS);
        }

        if (AboutKeys.ABOUT_DEFAULT_LANGUAGE.equals(key) && !m_supportedLanguages.contains(newValue.toString()))
        {
            throw new PropertyStoreException(PropertyStoreException.UNSUPPORTED_LANGUAGE);
        }

        languageTag = validateLanguageTag(languageTag, property);
        property.setValue(languageTag, newValue);

        setDefaultLanguageFromProperties();
        // save config map to persistent storage
        storeConfiguration();
    }

    /**
     * Reset the property value for a given language
     * 
     * @param key
     *            the property key
     * @param languageTag
     *            the language in which to reset
     * @throws PropertyStoreException
     */
    @Override
    public void reset(String key, String languageTag) throws PropertyStoreException
    {
        if (!Property.NO_LANGUAGE.equals(languageTag) && !m_supportedLanguages.contains(languageTag))
        {
            throw new PropertyStoreException(PropertyStoreException.UNSUPPORTED_LANGUAGE);
        }

        Property property = m_aboutConfigMap.get(key);
        if (property == null)
        {
            throw new PropertyStoreException(PropertyStoreException.UNSUPPORTED_KEY);
        }

        languageTag = validateLanguageTag(languageTag, property);
        property.remove(languageTag);

        // save config map to persistent storage
        storeConfiguration();
        loadFactoryDefaults();
        loadStoredConfiguration();

        // since the default language may be reset
        if (AboutKeys.ABOUT_DEFAULT_LANGUAGE.equals(key))
        {
            setDefaultLanguageFromProperties();
        }
    }

    /**
     * Reset all the properties in the store
     * 
     * @throws PropertyStoreException
     */
    @Override
    public void resetAll() throws PropertyStoreException
    {
        // delete cache
        Property appId = m_aboutConfigMap.get(AboutKeys.ABOUT_APP_ID);
        m_aboutConfigMap.clear();
        // delete persistent storage
        m_context.deleteFile(CONFIG_XML);
        // load factory defaults
        loadFactoryDefaults();
        // TODO restart as a soft AP...
        m_aboutConfigMap.put(AboutKeys.ABOUT_APP_ID, appId);
        loadLanguages();
    }

    /**
     * Checks if received languageTag is not {@link Property#NO_LANGUAGE} and
     * exists among supported languages, otherwise
     * {@link PropertyStoreException#UNSUPPORTED_LANGUAGE} is thrown. If the
     * received languageTag is {@link Property#NO_LANGUAGE} then it will be set
     * to the default language. If languages attribute of the received property
     * has only one language and it's set to {@link Property#NO_LANGUAGE}, then
     * returned languageTag will be set to {@link Property#NO_LANGUAGE}.
     * 
     * @param languageTag
     *            The language tag to be validates.
     * @param property
     *            The {@link Property} that the language tag is validated for.
     * @return The language tag to be used.
     * @throws PropertyStoreException
     *             of {@link PropertyStoreException#UNSUPPORTED_LANGUAGE}
     */
    private String validateLanguageTag(String languageTag, Property property) throws PropertyStoreException
    {

        if (!Property.NO_LANGUAGE.equals(languageTag) && !m_supportedLanguages.contains(languageTag))
        {
            throw new PropertyStoreException(PropertyStoreException.UNSUPPORTED_LANGUAGE);
        }

        Set<String> langs = property.getLanguages();

        // If languageTag equals NO_LANGUAGE, set it to be defaultLanguage
        if (Property.NO_LANGUAGE.equals(languageTag))
        {
            languageTag = m_defaultLanguage;
        }

        // In case the field has only one language and it equals NO_LANGUAGE,
        // there
        // will be no possibility to set another value with a different language
        // but NO_LANGUAGE.
        if (langs != null && langs.size() == 1)
        {

            Iterator<String> iterator = langs.iterator();
            String lang = iterator.next();

            if (Property.NO_LANGUAGE.equals(lang))
            {

                // Override the original language tag with the NO_LANGUAGE
                languageTag = Property.NO_LANGUAGE;
            }
        }

        return languageTag;
    }

    @Override
    public Map<String, Variant> getAboutData(String lang) throws ErrorReplyBusException
    {
        Map<String, Object> aboutData = new HashMap<String, Object>(3);
        getAbout(lang, aboutData);
        return TransportUtil.toVariantMap(aboutData);
    }

    @Override
    public Map<String, Variant> getAnnouncedAboutData() throws ErrorReplyBusException
    {
        Map<String, Object> announceData = new HashMap<String, Object>(3);
        getAnnouncement(m_defaultLanguage, announceData);
        return TransportUtil.toVariantMap(announceData);
    }
}
