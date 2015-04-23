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

package org.alljoyn.ioe.controlpanelservice.communication.interfaces;

import java.util.Map;

import org.alljoyn.bus.BusException;
import org.alljoyn.bus.Variant;
import org.alljoyn.ioe.controlpanelservice.ui.ajstruct.ListPropertyWidgetRecordAJ;

/**
 * List property super interface 
 */
public interface ListPropertyControlSuper {
	
	/**
	 * @return Interface version
	 */
	public short getVersion() throws BusException;

	/**
	 * @return States bitmask 
	 * @throws BusException
	 */
	public int getStates() throws BusException; 

	/**
	 * @return Optional parameters
	 * @throws BusException
	 */
	public Map<Short,Variant> getOptParams() throws BusException;
	
	/**
	 * @return The array of list records
	 * @throws BusException
	 */
	public ListPropertyWidgetRecordAJ[] getValue() throws BusException;
	
	/**
	 * Prepare the input form for adding a new record to the list
	 * @throws BusException
	 */
	public void Add() throws BusException;
	
	/**
	 * Prepare the form for view the record prior to the delete action.
	 * @param recordId
	 * @throws BusException
	 */
	public void Delete(short recordId) throws BusException;
	
	/**
	 * Prepare the display form to view the record identified by the recordID.
	 * @param recordId
	 * @throws BusException
	 */
	public void View(short recordId) throws BusException;
	
	/**
	 * Prepare the input form to view the record identified by the recordID and allow the end-user to modify the fields.
	 * @param recordId
	 * @throws BusException
	 */
	public void Update(short recordId) throws BusException;
	
	/**
	 * Confirm the action and save the change requested
	 * @throws BusException
	 */
	public void Confirm() throws BusException;
	
	/**
	 * Cancel the current action
	 * @throws BusException
	 */
	public void Cancel() throws BusException;
	
	/**
	 * The signal is sent when a list property value has changed, i.e. a record was added or removed from the records list
	 * @throws BusException
	 */
	public void ValueChanged() throws BusException;
	
	/**
	 * The signal is sent when there is a change in the list property metadata
	 * @throws BusException
	 */
	public void MetadataChanged() throws BusException;	
}
