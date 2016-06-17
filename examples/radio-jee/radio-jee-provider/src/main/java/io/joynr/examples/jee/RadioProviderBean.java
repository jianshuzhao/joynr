package io.joynr.examples.jee;

/*
 * #%L
 * %%
 * Copyright (C) 2011 - 2016 BMW Car IT GmbH
 * %%
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *      http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * #L%
 */

import java.util.Set;

import javax.ejb.Stateless;
import javax.inject.Inject;

import org.apache.commons.lang.math.RandomUtils;

import io.joynr.jeeintegration.api.ServiceProvider;
import io.joynr.provider.SubscriptionPublisherInjection;
import joynr.exceptions.ApplicationException;
import joynr.vehicle.RadioStation;
import joynr.vehicle.RadioSubscriptionPublisher;
import joynr.vehicle.RadioSync;

/**
 * Sample implementation of the {@link RadioSync} interface.
 */
@Stateless
@ServiceProvider(serviceInterface = RadioSync.class)
public class RadioProviderBean implements RadioSync, SubscriptionPublisherInjection<RadioSubscriptionPublisher> {

    private RadioStationDatabase radioStationDatabase;

    private GeoLocationService geoLocationService;

    @Inject
    public RadioProviderBean(RadioStationDatabase radioStationDatabase, GeoLocationService geoLocationService) {
        this.radioStationDatabase = radioStationDatabase;
        this.geoLocationService = geoLocationService;
    }

    @Override
    public RadioStation getCurrentStation() {
        return radioStationDatabase.getCurrentStation();
    }

    @Override
    public void shuffleStations() {
        Set<RadioStation> radioStations = radioStationDatabase.getRadioStations();
        int randomIndex = RandomUtils.nextInt(radioStations.size());
        radioStationDatabase.setCurrentStation(radioStations.toArray(new RadioStation[0])[randomIndex]);
    }

    @Override
    public Boolean addFavoriteStation(RadioStation newFavoriteStation) throws ApplicationException {
        radioStationDatabase.addRadioStation(newFavoriteStation);
        return true;
    }

    @Override
    public GetLocationOfCurrentStationReturned getLocationOfCurrentStation() {
        RadioStation currentStation = radioStationDatabase.getCurrentStation();
        GetLocationOfCurrentStationReturned result = null;
        if (currentStation != null) {
            result = new GetLocationOfCurrentStationReturned(currentStation.getCountry(),
                                                             geoLocationService.getPositionFor(currentStation.getCountry()));
        }
        return result;
    }

    @Override
    public void setSubscriptionPublisher(RadioSubscriptionPublisher subscriptionPublisher) {
        // Not supported by JEE integration yet, but necessary to have this method
        // so that the application can be deployed, because the radio.fidl has
        // subscription functionality in it.
    }

}
