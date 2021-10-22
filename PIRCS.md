# The Public Invention Respiratory Control Standard (PIRCS) v0.1

-- Robert L. Read, Geoff Mulligan, Lauria Clarke and Ben Coombs

## License

This document and standard are released under the Creative Commons CC0
license. However, you may not name any modification of this standard
the "PIRCS" or "Public Invention Respiratory Control Standard".
If you make a valuable extension or modification, we hope you will
submit it back to us, and if we agree, we will auugment the
standard and change the version number.

## Introduction

This document defines a standard, the Public Invention Respiratory Control Standard (PIRCS). The purpose of the PIRCS is to address the COVID-19 pandemic and more generally Composable Ventilator Systems by allowing the engineering community to design, test, manufacture and deploy a clinically effective, supply-chain resilient pandemic ventilation system.

The fundamental purpose of the PIRCS standard is to allow the “air drive” -- the piece which drives air and medical gases into the patient’s lungs -- in carefully controlled ways to be considered a module of a ventilation system in which different air drives could be used interchangeably. The goal is to separate the problem of producing carefully controlled pressure-drive air/oxygen mixtures from the other important problems in a functioning ventilator, such as user control, monitoring, alarming, air heating and humidification, and sanitization/treatment of exhaled air.

Because this is a global problem and because the global supply chain is distressed and clinical needs are evolving, there will not be one solution to the problem of supplying pandemic ventilators. We go further and assert there should not even be pandemic ventilators, but rather there should be pandemic ventilation systems. If you cannot obtain one kind of air source, you should be able to use a different kind of air source without retraining or changing the user display and control mechanism. Although other modularity potentially exists, this standard only encapsulates the “air drive” module. For example, a sister standard already in use, the Public Invention Respiratory Data Standard (PIRDS), is used for the testing/monitoring/alarming piece.

## The General Problem

As of April 24th, there are at least 75 teams working on building ventilators. They use fans, pumps, pressure regulators, and AmbuBags as basic drive mechanisms. Each of these systems have engineering, usage, and economic tradeoffs. The globe needs multiple solutions in each category.
The doctors and patients do not care how the air pushed. They only care that it is done in a reliable, controlled, and controllable manner. With a standard for controlling air drives and sufficient verification and quality control, we can  even imagine an AmuBag drive being unplugged and replaced with, for example, a pump, while the machine is paused for a few seconds without disturbing the patient or requiring resetting the control system or retraining the health care providers.

The fundamental problem requirement is to deliver air at a certain oxygen fraction within pressure limits of sufficient volume and at a sufficient rate, overcoming the compliance and resistance of the diseased lungs. This is expressed in a variety of ventilation modes. A useful air drive need not support all ventilation modes, but it must always deliver air within limits set by a clinician, within a small amount of error.

Because this must be controllable, this is both a data standard and an electrical standard. Systems which claim to conform to this standard must be plug-compatible with a system that controls them. Such a system should also be plug-compatible with a test machine which will control and evaluate the air drive over a long period of time. The standard thus facilitates the difficult problem of testing any proposed air drive sufficiently for clinicians and approval agencies to gain confidence in it.

## The Standard

An Air Drive will present an I2C or SPI electrical interface with a 4-pin JST or Molex-style connector. (size TBD.)
An Air Drive must clearly state what voltage level its outside interface operates at (5V, 3.3V, 1.8V or other.)
The non-communication eletrical power needed by the Air Drive is not a part of this standard.

## Event based protocol

PIRCS data is a series of events. It has no headers, though some events provide meta-information and may be grouped
together to form a header.

When possible, we follow the principle of making data human-readable if possible. In particular, because this
is meant in part for medical professionals, some unit types are chosen to conform that field.

The protocol defines an "acknowledgement" as a particular kind of message which acknowledges the request by echoing
the same data with two additional fields: An error number (zero if no error) and an status character. The protocol
does not state how the acknowledgemnt is to be tranported, but a preferred embodiment is using the same mechanism
by which the PIRCS command was transported in duplex. This will probably a serial port, but it could be an SPI communication
or some other means.

##  Control Command

The most common PIRCS data are command control events.

Commands are defined with 3 bytes [Setting, Interpretation, Value] with each byte followed by a 32-bit signed integer representing the value of each command. Where necessary, the value is multiplied by a decimal to allow an integer to express the acceptable range.

Integers are stored in "Big-Endian" byte order within their 4 bytes.

The parameters are:

1. M : Mode set as a char value listed below based on "int"
2. P : Target Pressure, cm H2O (a medical standard) times 10
4. E : PEEP Pressure, cm H20
3. V : Target Volume in milliliters
4. F : Target Flow rate, milliliters per second
5. B : Breaths per minute times 10
6. I : E:I Ratio, times 10 (NOTE: I:E ratio is typical, but this standard uses E:I!)
7. O : Oxygen FiO2, % times 10
8. S : Emergency Stop (TBD)
9. A : Alarm Control (TBD)

The interpretations are:

1. m : Minimum
2. M : Maximum
3. T : Target

Except when the parameter is "M", in which case the interpretation holds the character described belows. This allows the "val"
field to be universally an integer, and "int" field to be universally a single character.

### Modes

For the mode type, the third byte is the Ventilation Mode byte. There are a wide variety of ventilation modes and names are not entirely standardized.
These are set by using the command "mode" (M) and setting the character
below as the "int" or interpretation value to one of the characters below.
This document is the beginning of a particular standardization of the most common and easy to implement modes. The following modes are defined as part of the standard:

1. “C” - CPAP mode. The drive applies the pressure defined in the maximum/plateau pressure byte (byte 3) continuously. (Note: This mode can be used for testing compliance in certain useful but artificial (non-clinical) situations.)
1. “B” - BiPAP mode. The drive applies the maximum/plateau pressure for the inhalation phase, and at the PEEP pressure for exhalation. (Note: a more advanced mode will allow spontaneous breathing and volume control.
1. “V” -- Pressure Regulated Volume controlled ventilation. Within pressure limits, the machine provides the specified tidal volume with each breath.
1. “P” -- Pressure Ventilation Control mode : a simple mode of holding a constant input pressure.
1. “S” -- Spontaneous Breathing Mode. The drive pauses after the exhalation period waiting to receive an initiation command, but in all cases will begin a breath within the specified number of seconds. If the maximum number of seconds is 255, a mandatory breath is not required only patient-triggered breaths are required.
1. “R” -- Pressure Support Mode: patient initiates breaths, but pressure is automatically maintained at the set pressure level.
1. “A” -- Pressure Assist Mode: (Note: I’m not sure what the difference between this and “P” is.)
1. “I” - SIMV mode.
1. "s" -- emergency stop mode
1. "c" -- continue from emergency stop mode
1. "1" -- "one breath mode" --- for engineering, take a single breath
1. "h" -- "home" the machine for maximum readiness. Enter emergency stop mode.
1. Modes equal to and above 128 are defined by the drive.

### Control Command

# JSON Expression

Although driven by a need for a byte-level protocol to communicate electronically, there is
also need for a JSON-level expression of the standard. The obvious approach is to define
an event as a JSON object. We hope to make it somewhat human readable, but maintain a
direct connection to the Byte Level expression of the PIRCS standard.

Our expression will use JSON objects with slightly more information names. The character
codes will be used as is. The values will use the same
scale as defined in the byte level specifcations. This means that no floating point number will
at present ever appear in a value field.

So for example, to set the target pressure to 40.0 cmH20:

```JavaScript
{ "com": "C",
  "par" : "P",
  "int" : "T",
  "mod" : 0,
  "val" : 400
  }
```
This sets us the Mode to "Pressure Control Ventilation Mode"
```JavaScript
{ "com": "C",
  "par" : "M",
  "int" : "P",
  "mod" : 0,
  "val" : 0
  }
```

The "mod" parameter is a single unsigend integer between 0 and 255.

# Acknowledgements

Because we do not want to force microcontrolllers to fully implment JSON, we stick to the simplest possible
JSON compatible embodiment, and add two fields to form an acknowldgemt that directly copies the data sent.
Thus the acknowledgent for the command above is:
```JavaScript
{ "ack": "S",
  "err": 0,
  "com": "C",
  "par" : "P",
  "int" : "T",
  "mod" : 0,
  "val" : 400
  }
```
1. The "ack" or acknowledgment field is a single character "S" for successful reception and execution. If the ack field
is anything other than "S", the means something went wrong. Specific meanings may be added later or remain
implementation-specific will still conforming to this standard.
1. The "err" field contains a 32-bit unsigned integer value of meaning unspecified by this standard.

# Patient Identification and description commands

This is a new feature, added on September 25th, 2021.

Dr. Erich Schulz pointed out a need for the ability to uniquely identify a patient (real or simulated)
returned by the sibling protocol PIRDS, and that this can be accomplished by creating a command
to set it.

We therefore create a new class of PIRCS instructions, which we will call "Guidance" and denote
with "G", to distinguish from the commands, denoted by "C".

The first type of guidance is a patient identifying guidance. It deviates from the
short, fixed-size form (in terms of byte-buinding) of the command. In JavaScript, the
paradigm is:

```JavaScript
{ "com": "G",
  "par" : "I",
  "val" : "forty character string goes here"
  "sht" : 492
  }
```
The "G" stands for Guidance and the "I" stands for "Identifcation". A software system
receiving PIRCS on listening channel devoted to a single patient may take such guidance
as sufficient to store this 40-character string and return it as a patient ID.

The "sht" field is a 32-bit unsigned integer field used as a "shorthand" for the 40-character
val, so that PIRDS commands by kept to a reasonable length. Both the sender and the receiver
should interpret the shorthand as a temporary code for the full-length val.

Our intention is that the "sht" field will be added to the PIRDS commands, to separate
data from individual patients if multiple patient data streams are unable to be separated
any other way.

The second type of guidance is guidance about the patient condition.
This guidance is given intentional meaning by this standard.
However, the use or non-use of this guidance is left to the machine
interpreting the guidance. For example, it may change its behavior
in response to a known disease condition or the size of the patient.
The behavior fo the devices in response to guidance is outside the scope
of this standard.

The paradigm for guidance is the same as the paradigm for the command,
and uses the same type structure. "par" and "int" are single characters,
and the "mod" parameter is a single unsigend integer between 0 and 255.

```JavaScript
{ "com": "G",
  "par" : "X",
  "int" : "Y",
  "mod" : 0,
  "val" : 45969,
  }
```

The parameters set with "par" command are different in guidance ("G") than
they are in commands ("C"). They are:

1. W : Weight of the patient in grams.
1. H : Height of the patient in mm.
1. G : Gender of the patient. The "int" field should be "M" for male, "F" for female. Other genders are permissible
but are not defined by this standard.
1. D : Disease condition. (Please see below for the meaning of the "par" parameter in this case.). When
a disease condition is specified, the "val" field should contain a number betwen 0 and 100 representing the
severity of the condition. The meaning of this severity is not defined by this standard, but it should
be a monotone increasing function of disease condition (that is, higher when the condition is more severe.)

For example, to assert the patient is a male weighing 106 kilograms and 178 cm, the three independent
pieces of guidance might be:

```JavaScript
{ "com": "G",
  "par" : "W",
  "int" : "X",
  "mod" : 0,
  "val" : 106000,
  }
```
Note: Although it would be unusual to measure weight of a human being in grams, we provide this
precision for the possibility of neonatal patients. The PIRDS and PIRCS standards never
use floating point numbers to avoid the risk of floating-point induces mathematical error
and because microprocessing equipment may not be able to produce them easily.

```JavaScript
{ "com": "G",
  "par" : "H",
  "int" : "X",
  "mod" : 0,
  "val" : 1780,
  }
```

```JavaScript
{ "com": "G",
  "par" : "G",
  "int" : "M",
  "mod" : 0,
  "val" : 0,
  }
```


The currently recognized disease condtions are:
1. "D" : Disconnect. This is a detection that disconnect of the airway has occured. This should likely
be treated as a severe condition in all cases except a severity of "0" which may be used to assert
the absence of a disconnect.
1. "C" : Coughing or straining. This indicates patient-ventilator asynchrony in the form of coughing
detected by whatever means. A subsequent guidance may be provided when the patient quits coughing.
1. "S" : Asthma. The patient is asthmatic with the indicated severity.
1. "E" : Emphysema. The patient is emphysematic with the indicated severity.
1. "A" : ARDS. Acute respiratory distress syndrome. At present this standard makes no attempt
to distinguish various types of ARDS.
1. "P" : Pneumothorax. Pneumothorax is a life-threatening emergency condition.

For example, to indicate that the patient is asthmatic, the following guidance could be used:

```JavaScript
{ "com": "G",
  "par" : "S",
  "int" : "X",
  "mod" : 0,
  "val" : 75,
  }
```

# Proposed Parameter Enactment Mode

In general when interacting with a ventilator a single PIRCS expression represents asserting a single parameter value.
However a ventilator in operation must seamlessly affect such changes (for example, after the completion of the current breath.)
A clinician may in fact want to change several parameters simultaneously, and not change one until all can be changed.
This could be accomplished by allowing a set of PIRCS control expressions to be grouped (easily done as a JSON array.)

However, another way to do this is is to enter an "accumulation moded" with PIRCS command, which would then be "enacted"
at the end of a breath cycle after giving  an "enact" command.

Both approaches have advantages. In a future versions of this protocol we will implement one or both.

# License

Released under a Creative Commons 0 (CC0) Universal License. All rights to the marks "PubInv" and "Public Invention" reserved.
