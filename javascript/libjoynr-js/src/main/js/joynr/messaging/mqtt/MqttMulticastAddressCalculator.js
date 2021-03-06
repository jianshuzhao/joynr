/*jslint es5: true, nomen: true, node: true */

/*
 * #%L
 * %%
 * Copyright (C) 2011 - 2017 BMW Car IT GmbH
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
var Typing = require("../../util/Typing");
var MqttAddress = require("../../system/RoutingTypes/MqttAddress");

/**
 * @constructor MqttMulticastAddressCalculator
 * @param {Object}
 *            settings
 * @param {WebSocketAddress}
 *            settings.globalAddress
 */
var MqttMulticastAddressCalculator = function MqttMulticastAddressCalculator(settings) {
    Typing.checkProperty(settings, "Object", "settings");
    Typing.checkProperty(settings.globalAddress, "MqttAddress", "settings.globalAddress");
    this._settings = settings;
};

/**
 * Calculates the multicast address for the submitted joynr message
 * @function MqttMulticastAddressCalculator#calculate
 *
 * @param {JoynrMessage}
 *            message
 * @return {Address} the multicast address
 */
MqttMulticastAddressCalculator.prototype.calculate = function calculate(message) {
    return new MqttAddress({
        brokerUri: this._settings.globalAddress,
        topic: message.to
    });
};

module.exports = MqttMulticastAddressCalculator;
