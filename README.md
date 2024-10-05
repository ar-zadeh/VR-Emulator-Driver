# SteamVR Null Driver Installation and Usage Guide

This guide will walk you through the process of installing and using a new null driver in SteamVR. This driver allows you to emulate a headset and controllers without having the physical hardware connected.

## Step 1: Enable Null Driver in SteamVR

Before installing the new null driver, you need to enable the null driver feature in SteamVR:

1. Locate the SteamVR settings file:
   - Windows: `C:\Program Files (x86)\Steam\config\steamvr.vrsettings`
   - Linux: `~/.local/share/Steam/config/steamvr.vrsettings`
   - MacOS: `~/Library/Application Support/Steam/config/steamvr.vrsettings`

2. Open the file with a text editor.

3. Find the `"steamvr"` section in the JSON structure.

4. Add the following line within the `"steamvr"` section:
   ```json
   "activateMultipleDrivers": true,
   ```

5. Save and close the file.

## Step 2: Install the New Null Driver

1. Download the release file for the new null driver.

2. Locate your SteamVR driver directory:
   - Windows: `C:\Program Files (x86)\Steam\steamapps\common\SteamVR\drivers`
   - Linux: `~/.local/share/Steam/steamapps/common/SteamVR/drivers`
   - MacOS: `~/Library/Application Support/Steam/steamapps/common/SteamVR/drivers`

3. In the `drivers` folder, you should see a `null` folder. If it doesn't exist, create it.

4. Copy the contents of the release file into the `null` folder.

## Step 3: Interacting with the Driver

This null driver emulates a headset and two controllers. You can interact with them using your keyboard:

### Headset Control

The headset's position and rotation can be adjusted by editing two files:

1. `C:/actions/headset_position_changes.txt`: Controls the headset's position (X, Y, Z).
2. `C:/actions/headset_rotation_changes.txt`: Controls the headset's rotation (Yaw, Pitch, Roll).

Edit these files with the desired values, and the driver will read and apply the changes.

### Controller Control

The driver emulates two controllers. You can interact with them using the following keys:

#### Controller 1 (Left Hand):
- System: Q
- Application Menu: W
- Grip: E
- D-pad Left: R
- D-pad Up: T
- D-pad Right: Y
- D-pad Down: U
- A: I
- B: O
- X: P
- Y: A
- Trigger Click: S
- Trigger Value: D
- Trackpad Click: F
- Trackpad Touch: G
- Trackpad X: 2 (press and hold)
- Trackpad Y: 3 (press and hold)
- Trigger: X (press and hold)

#### Controller 2 (Right Hand):
- System: H
- Application Menu: J
- Grip: K
- D-pad Left: L
- D-pad Up: Z
- D-pad Right: X
- D-pad Down: C
- A: V
- B: B
- X: N
- Y: M
- Trigger Click: 1
- Trigger Value: 2
- Trackpad Click: 3
- Trackpad Touch: 4
- Trigger: 4 (press and hold)

The controllers' positions and rotations can be adjusted by editing these files:
- Controller 1:
  - Position: `C:/actions/controller1_position_changes.txt`
  - Rotation: `C:/actions/controller1_rotation_changes.txt`
- Controller 2:
  - Position: `C:/actions/controller2_position_changes.txt`
  - Rotation: `C:/actions/controller2_rotation_changes.txt`

Edit these files with the desired values, and the driver will read and apply the changes.

## Step 4: Using the Driver

1. Launch SteamVR.
2. The null driver should now be active, emulating your headset and controllers.
3. Use the keyboard controls described above to interact with the virtual devices.
4. Edit the position and rotation files to adjust the devices' positions and orientations in the virtual space.

## Troubleshooting

If you encounter any issues:
- Ensure SteamVR is completely closed before making changes.
- Double-check that the `activateMultipleDrivers` setting is correctly added to the `steamvr.vrsettings` file.
- Verify that all files are copied to the correct locations.
- Check SteamVR logs for any error messages related to driver loading.
- Make sure the position and rotation files exist and have the correct permissions.

For further assistance, please refer to the SteamVR community forums or contact the driver developer.
