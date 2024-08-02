#include "csamplecontrollerdriver.h"
#include "basics.h"
#include <fstream>
#include <sstream>
#include <math.h>
#include "driverlog.h"
#include "vrmath.h"

using namespace vr;

static double cyaw = 0, cpitch = 0, croll = 0;
static double cpX = 0, cpY = 0, cpZ = 0;
static double ct0, ct1, ct2, ct3, ct4, ct5;
static double c2yaw = 0, c2pitch = 0, c2roll = 0;

static double c2pX = 0, c2pY = 0, c2pZ = 0;

CSampleControllerDriver::CSampleControllerDriver()
{
    m_unObjectId = vr::k_unTrackedDeviceIndexInvalid;
    m_ulPropertyContainer = vr::k_ulInvalidPropertyContainer;
}

void CSampleControllerDriver::SetControllerIndex(int32_t CtrlIndex)
{
    ControllerIndex = CtrlIndex;
}

CSampleControllerDriver::~CSampleControllerDriver()
{
}

vr::EVRInitError CSampleControllerDriver::Activate(vr::TrackedDeviceIndex_t unObjectId)
{
    m_unObjectId = unObjectId;
    m_ulPropertyContainer = vr::VRProperties()->TrackedDeviceToPropertyContainer(m_unObjectId);

    vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, vr::Prop_ControllerType_String, "vive_controller");
    vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, vr::Prop_LegacyInputProfile_String, "vive_controller");

    vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, vr::Prop_ModelNumber_String, "ViveMV");
    vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, vr::Prop_ManufacturerName_String, "HTC");
    vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, vr::Prop_RenderModelName_String, "vr_controller_vive_1_5");

    vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, Prop_TrackingSystemName_String, "VR Controller");
    vr::VRProperties()->SetInt32Property(m_ulPropertyContainer, Prop_DeviceClass_Int32, TrackedDeviceClass_Controller);

    switch (ControllerIndex) {
    case 1:
        vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, Prop_SerialNumber_String, "CTRL1Serial");
        break;
    case 2:
        vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, Prop_SerialNumber_String, "CTRL2Serial");
        break;
    }

    uint64_t supportedButtons = 0xFFFFFFFFFFFFFFFFULL;
    vr::VRProperties()->SetUint64Property(m_ulPropertyContainer, vr::Prop_SupportedButtons_Uint64, supportedButtons);

    // return a constant that's not 0 (invalid) or 1 (reserved for Oculus)
    //vr::VRProperties()->SetUint64Property( m_ulPropertyContainer, Prop_CurrentUniverseId_Uint64, 2 );

    // avoid "not fullscreen" warnings from vrmonitor
    //vr::VRProperties()->SetBoolProperty( m_ulPropertyContainer, Prop_IsOnDesktop_Bool, false );

    // our sample device isn't actually tracked, so set this property to avoid having the icon blink in the status window
    //vr::VRProperties()->SetBoolProperty( m_ulPropertyContainer, Prop_NeverTracked_Bool, false );

    // even though we won't ever track we want to pretend to be the right hand so binding will work as expected

    switch (ControllerIndex) {
    case 1:
        vr::VRProperties()->SetInt32Property(m_ulPropertyContainer, Prop_ControllerRoleHint_Int32, TrackedControllerRole_LeftHand);
        break;
    case 2:
        vr::VRProperties()->SetInt32Property(m_ulPropertyContainer, Prop_ControllerRoleHint_Int32, TrackedControllerRole_RightHand);
        break;
    }

    // this file tells the UI what to show the user for binding this controller as well as what default bindings should
    // be for legacy or other apps
    vr::VRProperties()->SetStringProperty(m_ulPropertyContainer, Prop_InputProfilePath_String, "{null}/input/mycontroller_profile.json");

    //  Buttons handles
    vr::VRDriverInput()->CreateBooleanComponent(m_ulPropertyContainer, "/input/system/click", &HButtons[0]);
    vr::VRDriverInput()->CreateBooleanComponent(m_ulPropertyContainer, "/input/application_menu/click", &HButtons[1]);
    vr::VRDriverInput()->CreateBooleanComponent(m_ulPropertyContainer, "/input/grip/click", &HButtons[2]);
    vr::VRDriverInput()->CreateBooleanComponent(m_ulPropertyContainer, "/input/dpad_left/click", &HButtons[3]);
    vr::VRDriverInput()->CreateBooleanComponent(m_ulPropertyContainer, "/input/dpad_up/click", &HButtons[4]);
    vr::VRDriverInput()->CreateBooleanComponent(m_ulPropertyContainer, "/input/dpad_right/click", &HButtons[5]);
    vr::VRDriverInput()->CreateBooleanComponent(m_ulPropertyContainer, "/input/dpad_down/click", &HButtons[6]);
    vr::VRDriverInput()->CreateBooleanComponent(m_ulPropertyContainer, "/input/a/click", &HButtons[7]);
    vr::VRDriverInput()->CreateBooleanComponent(m_ulPropertyContainer, "/input/b/click", &HButtons[8]);
    vr::VRDriverInput()->CreateBooleanComponent(m_ulPropertyContainer, "/input/x/click", &HButtons[9]);
    vr::VRDriverInput()->CreateBooleanComponent(m_ulPropertyContainer, "/input/y/click", &HButtons[10]);
    vr::VRDriverInput()->CreateBooleanComponent(m_ulPropertyContainer, "/input/trigger/click", &HButtons[11]);
    vr::VRDriverInput()->CreateBooleanComponent(m_ulPropertyContainer, "/input/trigger/value", &HButtons[12]);
    vr::VRDriverInput()->CreateBooleanComponent(m_ulPropertyContainer, "/input/trackpad/click", &HButtons[13]);
    vr::VRDriverInput()->CreateBooleanComponent(m_ulPropertyContainer, "/input/trackpad/touch", &HButtons[14]);

    // Analog handles

    // Analog handles
    vr::VRDriverInput()->CreateScalarComponent(
                m_ulPropertyContainer, "/input/trackpad/x", &HAnalog[0],
            vr::EVRScalarType::VRScalarType_Absolute, vr::EVRScalarUnits::VRScalarUnits_NormalizedTwoSided
            );
    vr::VRDriverInput()->CreateScalarComponent(
                m_ulPropertyContainer, "/input/trackpad/y", &HAnalog[1],
            vr::EVRScalarType::VRScalarType_Absolute, vr::EVRScalarUnits::VRScalarUnits_NormalizedTwoSided
            );
    vr::VRDriverInput()->CreateScalarComponent(
                m_ulPropertyContainer, "/input/trigger/value", &HAnalog[2],
            vr::EVRScalarType::VRScalarType_Absolute, vr::EVRScalarUnits::VRScalarUnits_NormalizedOneSided
            );

    vr::VRProperties()->SetInt32Property(m_ulPropertyContainer, vr::Prop_Axis0Type_Int32, vr::k_eControllerAxis_TrackPad);

    // create our haptic component
    vr::VRDriverInput()->CreateHapticComponent(m_ulPropertyContainer, "/output/haptic", &m_compHaptic);

    return VRInitError_None;
}

void CSampleControllerDriver::Deactivate()
{
    m_unObjectId = vr::k_unTrackedDeviceIndexInvalid;
}

void CSampleControllerDriver::EnterStandby()
{
}

void *CSampleControllerDriver::GetComponent(const char *pchComponentNameAndVersion)
{
    // override this to add a component to a driver
    return NULL;
}

void CSampleControllerDriver::PowerOff()
{
}

void CSampleControllerDriver::DebugRequest(const char *pchRequest, char *pchResponseBuffer, uint32_t unResponseBufferSize)
{
    if (unResponseBufferSize >= 1) {
        pchResponseBuffer[0] = 0;
    }
}

vr::DriverPose_t CSampleControllerDriver::GetPose()
{

    vr::DriverPose_t pose = { 0 };
    pose.poseIsValid = true;
    pose.result = vr::TrackingResult_Running_OK;
    pose.deviceIsConnected = true;
    pose.qWorldFromDriverRotation = HmdQuaternion_Init(1, 0, 0, 0);
    pose.qDriverFromHeadRotation = HmdQuaternion_Init(1, 0, 0, 0);



    if (ControllerIndex == 1) {
        std::string posFileName = "C:/actions/controller1_position_changes.txt";
        std::string rotFileName = "C:/actions/controller1_rotation_changes.txt";
        // Read position changes from file
        std::ifstream posFile(posFileName);
        if (posFile.is_open()) {
            std::string line;
            std::getline(posFile, line);
            std::istringstream iss(line);
            double posChanges[3] = { 0 };
            iss >> posChanges[0] >> posChanges[1] >> posChanges[2];
            cpX += posChanges[0];
            cpY += posChanges[1];
            cpZ += posChanges[2];

            posFile.close();

            // Reset position changes to zero
            std::ofstream resetPosFile(posFileName);
            if (resetPosFile.is_open()) {
                resetPosFile << "0 0 0";
                resetPosFile.close();
            }
        }

        // Read rotation changes from file
        std::ifstream rotFile(rotFileName);
        if (rotFile.is_open()) {
            std::string line;
            std::getline(rotFile, line);
            std::istringstream iss(line);
            double rotChanges[3] = { 0 };
            iss >> rotChanges[0] >> rotChanges[1] >> rotChanges[2];
            cyaw += DEG_TO_RAD(rotChanges[0]);
            cpitch += DEG_TO_RAD(rotChanges[1]);
            croll += DEG_TO_RAD(rotChanges[2]);
            rotFile.close();
            DriverLog("cyaw = %.2f", cyaw);
            DriverLog("cyaw = %.2f", cpitch);
            DriverLog("cyaw = %.2f", croll);

            // Reset rotation changes to zero
            std::ofstream resetRotFile(rotFileName);
            if (resetRotFile.is_open()) {
                resetRotFile << "0 0 0";
                resetRotFile.close();
            }
        }


        // Convert yaw, pitch, roll to quaternion
        ct0 = cos(cyaw * 0.5);
        ct1 = sin(cyaw * 0.5);
        ct2 = cos(croll * 0.5);
        ct3 = sin(croll * 0.5);
        ct4 = cos(cpitch * 0.5);
        ct5 = sin(cpitch * 0.5);

        pose.qRotation.w = ct0 * ct2 * ct4 + ct1 * ct3 * ct5;
        pose.qRotation.x = ct0 * ct3 * ct4 - ct1 * ct2 * ct5;
        pose.qRotation.y = ct0 * ct2 * ct5 + ct1 * ct3 * ct4;
        pose.qRotation.z = ct1 * ct2 * ct4 - ct0 * ct3 * ct5;

        pose.vecPosition[0] = cpX;
        pose.vecPosition[1] = cpY;
        pose.vecPosition[2] = cpZ;
    }
    else {
        std::string posFileName = "C:/actions/controller2_position_changes.txt";
        std::string rotFileName = "C:/actions/controller2_rotation_changes.txt";

        // Read position changes from file
        std::ifstream posFile(posFileName);
        if (posFile.is_open()) {
            std::string line;
            std::getline(posFile, line);
            std::istringstream iss(line);
            double posChanges[3] = { 0 };
            iss >> posChanges[0] >> posChanges[1] >> posChanges[2];
            c2pX += posChanges[0];
            c2pY += posChanges[1];
            c2pZ += posChanges[2];

            posFile.close();

            // Reset position changes to zero
            std::ofstream resetPosFile(posFileName);
            if (resetPosFile.is_open()) {
                resetPosFile << "0 0 0";
                resetPosFile.close();
            }
        }

        // Read rotation changes from file
        std::ifstream rotFile(rotFileName);
        if (rotFile.is_open()) {
            std::string line;
            std::getline(rotFile, line);
            std::istringstream iss(line);
            double rotChanges[3] = { 0 };
            iss >> rotChanges[0] >> rotChanges[1] >> rotChanges[2];
            c2yaw += DEG_TO_RAD(rotChanges[0]);
            c2pitch += DEG_TO_RAD(rotChanges[1]);
            c2roll += DEG_TO_RAD(rotChanges[2]);
            rotFile.close();
            DriverLog("cyaw = %.2f", c2yaw);
            DriverLog("cyaw = %.2f", c2pitch);
            DriverLog("cyaw = %.2f", c2roll);

            // Reset rotation changes to zero
            std::ofstream resetRotFile(rotFileName);
            if (resetRotFile.is_open()) {
                resetRotFile << "0 0 0";
                resetRotFile.close();
            }
        }


        // Convert yaw, pitch, roll to quaternion
        ct0 = cos(c2yaw * 0.5);
        ct1 = sin(c2yaw * 0.5);
        ct2 = cos(c2roll * 0.5);
        ct3 = sin(c2roll * 0.5);
        ct4 = cos(c2pitch * 0.5);
        ct5 = sin(c2pitch * 0.5);

        pose.qRotation.w = ct0 * ct2 * ct4 + ct1 * ct3 * ct5;
        pose.qRotation.x = ct0 * ct3 * ct4 - ct1 * ct2 * ct5;
        pose.qRotation.y = ct0 * ct2 * ct5 + ct1 * ct3 * ct4;
        pose.qRotation.z = ct1 * ct2 * ct4 - ct0 * ct3 * ct5;

        pose.vecPosition[0] = c2pX;
        pose.vecPosition[1] = c2pY;
        pose.vecPosition[2] = c2pZ;
    }

    return pose;
}

void CSampleControllerDriver::RunFrame()
{
    // ... (existing code) ...

    if (ControllerIndex == 1) {
        vr::VRDriverInput()->UpdateBooleanComponent(HButtons[0], (0x8000 & GetAsyncKeyState('Q')) != 0, 0); // System
        vr::VRDriverInput()->UpdateBooleanComponent(HButtons[1], (0x8000 & GetAsyncKeyState('W')) != 0, 0); // Application Menu
        vr::VRDriverInput()->UpdateBooleanComponent(HButtons[2], (0x8000 & GetAsyncKeyState('E')) != 0, 0); // Grip
        vr::VRDriverInput()->UpdateBooleanComponent(HButtons[3], (0x8000 & GetAsyncKeyState('R')) != 0, 0); // D-pad Left
        vr::VRDriverInput()->UpdateBooleanComponent(HButtons[4], (0x8000 & GetAsyncKeyState('T')) != 0, 0); // D-pad Up
        vr::VRDriverInput()->UpdateBooleanComponent(HButtons[5], (0x8000 & GetAsyncKeyState('Y')) != 0, 0); // D-pad Right
        vr::VRDriverInput()->UpdateBooleanComponent(HButtons[6], (0x8000 & GetAsyncKeyState('U')) != 0, 0); // D-pad Down
        vr::VRDriverInput()->UpdateBooleanComponent(HButtons[7], (0x8000 & GetAsyncKeyState('I')) != 0, 0); // A
        vr::VRDriverInput()->UpdateBooleanComponent(HButtons[8], (0x8000 & GetAsyncKeyState('O')) != 0, 0); // B
        vr::VRDriverInput()->UpdateBooleanComponent(HButtons[9], (0x8000 & GetAsyncKeyState('P')) != 0, 0); // X
        vr::VRDriverInput()->UpdateBooleanComponent(HButtons[10], (0x8000 & GetAsyncKeyState('A')) != 0, 0); // Y
        vr::VRDriverInput()->UpdateBooleanComponent(HButtons[11], (0x8000 & GetAsyncKeyState('S')) != 0, 0); // Trigger Click
        vr::VRDriverInput()->UpdateBooleanComponent(HButtons[12], (0x8000 & GetAsyncKeyState('D')) != 0, 0); // Trigger Value
        vr::VRDriverInput()->UpdateBooleanComponent(HButtons[13], (0x8000 & GetAsyncKeyState('F')) != 0, 0); // Trackpad Click
        vr::VRDriverInput()->UpdateBooleanComponent(HButtons[14], (0x8000 & GetAsyncKeyState('G')) != 0, 0); // Trackpad Touch

        vr::VRDriverInput()->UpdateScalarComponent(HAnalog[0], 0.0, 0); //Trackpad x
        vr::VRDriverInput()->UpdateScalarComponent(HAnalog[1], 0.0, 0); //Trackpad y

        if ((GetAsyncKeyState('2') & 0x8000) != 0) {
            vr::VRDriverInput()->UpdateScalarComponent(HAnalog[0], 1.0, 0);
        }

        if ((GetAsyncKeyState('3') & 0x8000) != 0) {
            vr::VRDriverInput()->UpdateScalarComponent(HAnalog[1], 1.0, 0);
        }

        if ((GetAsyncKeyState('X') & 0x8000) != 0) { //Trigger
            vr::VRDriverInput()->UpdateScalarComponent(HAnalog[2], 1.0, 0);
        }
        else {
            vr::VRDriverInput()->UpdateScalarComponent(HAnalog[2], 0.0, 0);
        }
    }
    else {
        //Controller2
        vr::VRDriverInput()->UpdateBooleanComponent(HButtons[0], (0x8000 & GetAsyncKeyState('H')) != 0, 0); // System
        vr::VRDriverInput()->UpdateBooleanComponent(HButtons[1], (0x8000 & GetAsyncKeyState('J')) != 0, 0); // Application Menu
        vr::VRDriverInput()->UpdateBooleanComponent(HButtons[2], (0x8000 & GetAsyncKeyState('K')) != 0, 0); // Grip
        vr::VRDriverInput()->UpdateBooleanComponent(HButtons[3], (0x8000 & GetAsyncKeyState('L')) != 0, 0); // D-pad Left
        vr::VRDriverInput()->UpdateBooleanComponent(HButtons[4], (0x8000 & GetAsyncKeyState('Z')) != 0, 0); // D-pad Up
        vr::VRDriverInput()->UpdateBooleanComponent(HButtons[5], (0x8000 & GetAsyncKeyState('X')) != 0, 0); // D-pad Right
        vr::VRDriverInput()->UpdateBooleanComponent(HButtons[6], (0x8000 & GetAsyncKeyState('C')) != 0, 0); // D-pad Down
        vr::VRDriverInput()->UpdateBooleanComponent(HButtons[7], (0x8000 & GetAsyncKeyState('V')) != 0, 0); // A
        vr::VRDriverInput()->UpdateBooleanComponent(HButtons[8], (0x8000 & GetAsyncKeyState('B')) != 0, 0); // B
        vr::VRDriverInput()->UpdateBooleanComponent(HButtons[9], (0x8000 & GetAsyncKeyState('N')) != 0, 0); // X
        vr::VRDriverInput()->UpdateBooleanComponent(HButtons[10], (0x8000 & GetAsyncKeyState('M')) != 0, 0); // Y
        vr::VRDriverInput()->UpdateBooleanComponent(HButtons[11], (0x8000 & GetAsyncKeyState('1')) != 0, 0); // Trigger Click
        vr::VRDriverInput()->UpdateBooleanComponent(HButtons[12], (0x8000 & GetAsyncKeyState('2')) != 0, 0); // Trigger Value
        vr::VRDriverInput()->UpdateBooleanComponent(HButtons[13], (0x8000 & GetAsyncKeyState('3')) != 0, 0); // Trackpad Click
        vr::VRDriverInput()->UpdateBooleanComponent(HButtons[14], (0x8000 & GetAsyncKeyState('4')) != 0, 0); // Trackpad Touch

        vr::VRDriverInput()->UpdateScalarComponent(HAnalog[0], 0.0, 0); //Trackpad x
        vr::VRDriverInput()->UpdateScalarComponent(HAnalog[1], 0.0, 0); //Trackpad y

        if ((GetAsyncKeyState('4') & 0x8000) != 0) { //Trigger
            vr::VRDriverInput()->UpdateScalarComponent(HAnalog[2], 1.0, 0);
        }
        else {
            vr::VRDriverInput()->UpdateScalarComponent(HAnalog[2], 0.0, 0);
        }
    }

    if (m_unObjectId != vr::k_unTrackedDeviceIndexInvalid) {
        vr::VRServerDriverHost()->TrackedDevicePoseUpdated(m_unObjectId, GetPose(), sizeof(DriverPose_t));
    }

}
void CSampleControllerDriver::ProcessEvent(const vr::VREvent_t &vrEvent)
{
    switch (vrEvent.eventType) {
    case vr::VREvent_Input_HapticVibration:
        if (vrEvent.data.hapticVibration.componentHandle == m_compHaptic) {
            // This is where you would send a signal to your hardware to trigger actual haptic feedback
            //DriverLog( "BUZZ!\n" );
        }
        break;
    }
}

std::string CSampleControllerDriver::GetSerialNumber() const
{
    switch (ControllerIndex) {
    case 1:
        return "CTRL1Serial";
        break;
    case 2:
        return "CTRL2Serial";
        break;
    }
}


