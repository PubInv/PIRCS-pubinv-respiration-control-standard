# The Public Invention Respiratory Control Standard (PIRCS) v0.1

-- Robert L. Read, Geoff Mulligan, and Lauria Clarke

## License

This document and standard are released under the Creative Commons CC0
license. However, you may not name any modification of this standard
the "PIRCS" or "Public Invention Respiratory Control Standard".
If you make a valuable extension or modification, we hope you will
submit it back to us, and if we agree, we will augment the
standard and change the version number.

## Introduction

This document defines a standard, the Public Invention Respiratory Control Standard (PIRCS). The purpose of the PIRCS is to address the COVID-19 pandemic and more generally Composable Ventilator Systems by allowing the engineering community to design, test, manufacture and deploy a clinically effective, supply-chain resilient pandemic ventilation system.

The fundamental purpose of the PIRCS standard is to allow the “air drive” -- the piece which drives air and medical gases into the patient’s lungs -- in carefully controlled ways to be considered a module of a ventilation system in which different air drives could be used interchangeably. The goal is to separate the problem of producing carefully controlled pressure-drive air/oxygen mixtures from the other important problems in a functioning ventilator, such as user control, monitoring, alarming, air heating and humidification, and sanitization/treatment of exhaled air.

Because this is a global problem and because the global supply chain is distressed and clinical needs are evolving, there will not be one solution to the problem of supplying pandemic ventilators. We go further and assert there should not even be pandemic ventilators, but rather there should be pandemic ventilation systems. If you cannot obtain one kind of air source, you should be able to use a different kind of air source without retraining or changing the user display and control mechanism. Although other modularity potentially exists, this standard only encapsulates the “air drive” module. For example, a sister standard already in use, the Public Invention Respiratory Data Standard (PIRDS), is used for the testing/monitoring/alarming piece.

## The General Problem

As of April 24th, there are at least 75 teams working on building ventilators. They use fans, pumps, pressure regulators, and AmbuBags as basic drive mechanisms. Each of these systems have engineering, usage, and economic tradeoffs. The globe needs multiple solutions in each category.
The doctors and patients do not care how the air pushed. They only care that it is done in a reliable, controlled, and controllable manner. With a standard for controlling air drives and sufficient verification and quality control, we can even imagine an AmuBag drive being unplugged and replaced with, for example, a pump, while the machine is paused for a few seconds without disturbing the patient or requiring resetting the control system or retraining the health care providers.

The fundamental problem requirement is to deliver air at a certain oxygen fraction within pressure limits of sufficient volume and at a sufficient rate, overcoming the compliance and resistance of the diseased lungs. This is expressed in a variety of ventilation modes. A useful air drive need not support all ventilation modes, but it must always deliver air within limits set by a clinician, within a small amount of error.

Because this must be controllable, this is both a data standard and an electrical standard. Systems which claim to conform to this standard must be plug-compatible with a system that controls them. Such a system should also be plug-compatible with a test machine which will control and evaluate the air drive over a long period of time. The standard thus facilitates the difficult problem of testing any proposed air drive sufficiently for clinicians and approval agencies to gain confidence in it.

## The Standard

An Air Drive will present an I2C or SPI electrical interface with a 4-pin JST or Molex-style connector. (size TBD.)
An Air Drive must clearly state what voltage level its outside interface operates at (5V, 3.3V, 1.8V or other.) 
The non-communication electrical power needed by the Air Drive is not a part of this standard.


The air drive will listen on an address of (x??) (TBD) and will also listen for broadcast messages. A PIRCS v0.1 message will consist of precisely ten bytes. These bytes are clinically meaningful parameters. They are in order:

1. The capital letter “C”. The stands for “Command”.
1. A byte representing ventilation mode (see below for detail).
1. A byte representing maximum, peak, or plateau pressure in units of (unsigned integral cm H2O).
1. A byte representing minimum, or positive end-expiratory pressure (PEEP) in units of (unsigned integral cm H2O).
1. A byte representing Respiration Rate in units of (unsigned integral breaths per minute).
1. A byte representing desired Tidal Volume in units of (unsigned integral 10 milliliters). That is a value of 30 would mean Tidal Volume of 300 ml, and a value of 100 would mean tidal volume of 1000 milliliters.
1. A byte representing the I:E or Inhalation time to Exhalation time ration. The units of this are 10 divided by the prescribed I:E ratio (equivalently, 10 times the E:I ratio, the reciprocal of the commonly used I:E ratio.) Thus, a byte of 10 represents an I:E ratio of 1:1. A value of 100 represents an I:E ratio of 1:10.  A byte of 7 would represent the clinically unusual situation of a shorter exhalation time than the inhalation time, in a ratio of 10:7, or 1:0.7.
1. A byte representing the maximum time in seconds the drive will pause before initiating the next breath. Units are unsigned seconds. A value of 13 means the machine will initiate a new breath in no more than 13 seconds after the end of the previous exhalation. For various reasons the drive may initiate the breath sooner.
1. A byte representing the fraction of oxygen to be delivered, expressed as an unsigned percentage. A value of 100 would represent 100% oxygen. A value of 60 would represent 60% oxygen.
1. A tenth byte is unused and may be defined by the air drive to have any documented meaning it chooses. For example, a particular drive might offer warming of the gas as a controllable feature via this byte.

Additionally, there are 2 specifically defined byte sequences commands:

1. The byte “C” followed by the byte “I” for “initiate”  and 8 bytes of value 255 means “initiate a spontaneous breath immediately if you are not in a state of currently respiring”. The drive should expect 3 such commands to be issued, any of which can be ignored once a breathing state is entered. “Initiates” command given before the end of an exhalation or no-ops are to be ignored.
1. The byte “X” followed by a byte value T and 8 bytes of value 0 is the command for panic stop. The drive is to cease applying pressure immediately and not begin respiration again until T seconds have passed.

Except in these cases, the second byte is the Ventilation Mode byte. There are a wide variety of ventilation modes and names are not entirely standardized. This document is the beginning of a particular standardization of the most common and easy to implement modes. The following modes are defined as part of the standard:

1. “C” - CPAP mode. The drive applies the pressure defined in the maximum/plateau pressure byte (byte 3) continuously. (Note: This mode can be used for testing compliance in certain useful but artificial (non-clinical) situations.)
1. “B” - BiPAP mode. The drive applies the maximum/plateau pressure for the inhalation phase, and at the PEEP pressure for exhalation. (Note: a more advanced mode will allow spontaneous breathing and volume control).
1. “V” -- Pressure Regulated Volume controlled ventilation. Within pressure limits, the machine provides the specified tidal volume with each breath.
1. “S” -- Spontaneous Breathing Mode. The drive pauses after the exhalation period waiting to receive an initiation command, but in all cases will begin a breath within the specified number of seconds. If the maximum number of seconds is 255, a mandatory breath is not required only patient-triggered breaths are required.
1. “P” -- Pressure Support Mode: patient initiates breaths, but pressure is automatically maintained at the set pressure level.
1. “A” -- Pressure Assist Mode: (Note: I’m not sure what the difference between this and “P” is.)
1. “I” - SIMV mode.
1. Modes equal to and above 128 are defined by the drive.

At the time of this writing (April 28th, 2020) this list of nodes is poorly defined and not rich enough.
Improving these is the primary thing we have to do make this standard ready for use; we invite knowledgeable volunteers
to suggest improvements via issues of pull requests.

In all cases, the “air drive” must provide documentation as to what modes it supports, and any limitations it may have. For example, Dr. Erich Schulz has suggested that many current designs are underpowered. If a given machine cannot support a minute volume of more than 80 liters per minute, it should be so documented; this standard provides does not a means of capturing that limitation (thought test strategy may reveal it.)
