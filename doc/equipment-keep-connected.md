# Rig, Rotator and CW Keyer – keeping the connection

The connect button of the Rig, Rotator and CW Console widgets means
"I want this connected". QLog keeps the connection as long as the button is
switched on:

| Button colour | Meaning |
|---|---|
| none | switched off |
| **yellow** | switched on, not connected – QLog is trying |
| **green** | connected |

- When the connection cannot be opened, or is lost later (radio switched off,
  USB cable pulled, rigctld ended), the button turns yellow and QLog tries
  again after 5, 10, 20 and then every 30 seconds. No dialog interrupts the
  operator.
- The reason and the time of the next attempt are shown as the tooltip of the
  button and as a message in the status bar. The same text is written to the
  debug log.
- Switch the radio on (plug the cable in, start rigctld) and the button turns
  green by itself.
- A radio that is switched off but still reachable (typically behind
  rigctld) answers "Rig is not powered on". The button turns yellow with that
  reason, the link stays open, and it turns green the moment the radio is
  powered on. Nothing is reconnected in between.
- Switching the button off stops the attempts.

Together with the *Connect* option of a Rig profile in the Activity Manager
this means: start QLog before the radio, and the rig connects the moment it is
powered on.

QLog never sends a power-on command when it connects, so the attempts cannot
switch a radio on that the operator has turned off.
