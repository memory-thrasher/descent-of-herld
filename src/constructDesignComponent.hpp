/*
Copyright 2024-2025 Wafflecat Games, LLC

This file is part of The Descent of Herld.

The Descent of Herld is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

The Descent of Herld is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with The Descent of Herld. If not, see <https://www.gnu.org/licenses/>.

Stable and intermediate releases may be made continually. For this reason, a year range is used in the above copyrihgt declaration. I intend to keep the "working copy" publicly visible, even if it is not functional. I consider every push to this publicly visible repository as a release. Releases intended to be stable will be marked as such via git tag or similar feature.
*/

#pragma once

#include "connector.hpp"

namespace doh {

  //this might not belong here
  enum class componentSlotType_e : uint64_t {
    primary,//eg centrifuge
    secondary,//direct contact with primary (eg resistive heater)
    interior,//inside primary (eg agitator, piston head)
    driver,//responsible for primary's function (eg motor that drives a centrifuge)
    connector,//connects other slots, potentially allowing more of a certain type (eg shaft, gearbox)
    structure,//often supporting auxiliary components (eg iron girder)
    auxiliary,//near primary but not in direct contact (eg laser heater, magnetic containment, reinforcement)
    hull,//extremity of module (eg armor, access connector)
  };

  enum class componentType_e : uint64_t {
    #error TODO, see below for ideas
  };

  /*
  enum class moduleType_e : uint64_t {//too many, too specific. Want more modularity.
    //resource collection
    resource_scoop,
    resource_magneticGrapple,//starting equipment
    resource_mechanicalHarpoon,
    resource_gravityGrapple,
    resource_depositoryAndHammerCombo,//rocks delivered by drones or arms
    //crushers
    crusher_reciprocatingHammer = 1000,
    crusher_jaw,//near-parallel plate, requires acceleration or positive pressure
    crusher_flail,//spinning chains inside a drum
    crusher_rotatingShaft,//better for metal scrap than rock
    //centrifuges
    centrifuge_spider = 2000,
    centrifuge_drum,
    centrifuge_gravityChamber,//not really a centrifuge but solves the same niche using the same interface
    //other pumps and movers
    pump_vane = 3000,//circular motion of inner agitator with fins mechanically tracing the container's inner wall
    pump_liquidRing,
    pump_reciprocating,//piston
    pump_progressiveCavity,
    //chambers
    chamber_storage = 4000,
    chamber_heating,
    chamber_reaction,//including fire
    chamber_mixing,
    chamber_electrolyser,
    chamber_assembly,//arms, tools, potentially including many shaping operations
    chamber_modular,//tool slots for any/all of above chambers and more
    chamber_precisionMatterFabricator,//end-game, produces complete objects from energy without shaping or refining
    //shaping
    shaping_mill = 5000,//drill bits
    shaping_extruder,//solid/liquid/both depends on composition
    shaping_roller,//configuration of rolls for bar, wire, tube, plate, bent plate, dye shaping etc
    shapping_hammer,//for plate or dye pressing
    //containerizers
    containerizer_spool = 6000,
    containerizer_barrel,
    containerizer_pallet,
    containerizer_universalCell,
    containerizer_batteryCharger,
    //connectors
    connector_beamTube = 7000,//mirros at elbows, for lasers and accelerated particles
    connector_beamway,//mirros at elbows, otherwise open, for lasers only
    connector_pipe,
    connector_wire,
    connector_cable,//multi-conductor wire, nonstructural
    connector_modular,
    connector_kinetic,//rod/shaft fixed/bowden, gearboxes at corners if fixed
    connector_monorail,
    connector_pneumaticTube,//or hydraulic
    connector_wormhole,//late game, mated pairs and high energy at first, improving with precision
    //flow control
    flow_sieve = 6000,
    flow_sorter,
    flow_heatExchanger,
    flow_ballValve,
    flow_pistonValve,
    flow_regulator,//pressure, flow, total quantity etc, capabilities and precision based on modules
    //power
    power_photovoltaic = 7000,
    power_radiovoltaic,//capturing free electrons and other charged particles
    power_rotation,
    power_linear,
    power_electrolyticReactor,//dis/charge with anode and cathode piped in and out
    power_totalConversion,//matter -> electrical power without heat generation
    //kinetic rotation
    rotation_electromagnetic = 8000,
    rotation_internalCombustion,
    rotation_turbineBladeless,//a la Tesla
    rotation_turbineBladed,//traditional
    //linear travel (generally reciprocating)
    linear_solenoid = 9000,//oscillating from electric
    linear_leadScrew,//precision from rotational
    linear_piston,//from pressure/flow
    linear_crankShaft,//oscillating from rotational
    //thermal
    thermal_resistiveHeater = 10000,
    thermal_dissipator,//light and/or other radiation
    thermal_electrocoupler,
    //radio
    radio_gasChamber = 11000,
    radio_ledArray,
    radio_waveAntenna,
    radio_linearAccelerator,
    radio_loopAccelerator,
    //vents
    vent_directedGas = 12000,//thruster
    vent_cappedGas,//equal force in opposing directions = no thrust
    vent_massEjector,//thrust, technically. Accepts linear travel
    vent_massEjectorOpposing,
    //navigation
    navigation_gyroscope = 13000,
    //sensors
    //weapons and mining utilities
    harpoon = 14000,
    //TODO more
  };//TODO particle accelerator, in what category?
  */

  struct constructDesignComponent {
    uint64_t owner;//either a constructDesign, or a construct (if this is a data member of constructComponent)
    static constexpr size_t noteMaxChars = 64;
    char note[noteMaxChars+1];
    componentType_e componentType;
    uint64_t subtype;
    uint64_t tier;//?
    //TODO alloy
  };

}
