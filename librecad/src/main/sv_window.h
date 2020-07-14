#ifndef SV_WINDOW_H
#define SV_WINDOW_H


#include <qwidget.h>
#include <QPushButton>
#include <QLabel>
#include <QListWidget>
#include <QDateTime>
#include <QObject>
#include <QtDebug>
#include <QTimer>
#include "qc_applicationwindow.h"
//#include <QtSerialPort/QSerialPort>
#include "settingsdialog.h"
#include "sv_logdialog.h"
#include <QLocalServer>
#include <QQueue>
#include <QMutex>
#include <QToolButton>
#include "sv_displayconnectstatus.h"
#include "sv_displaytime.h"
#include "sv_displayeventstatus.h"
#include <QThread>
#include "workerbackupdb.h"
#include "concurrentqueue.h"
#include <QMediaPlayer>
#include "sv_label.h"
#include "respworker.h"
#include "commthread.h"
#include "mgwip.h"
#include "sv_eventlist.h"

/*

#include <QListWidgetItem>
#include <QTranslator>


*/
class QWidget;
class QLabel;
class QListWidget;
class QObject;
class SettingsDialog;


class SV_Window : public QWidget{
	Q_OBJECT

public:
	
	enum AlertType {
			Alert_Fire,
			Alert_R4,
			Alert_Fault,
		};
	
	enum WindowState {
			Window_Unknown,
			Window_Opened,
			Window_Closed
		};

signals:
	void svclose();	
	void addCmdQueue(const QString &cmd);
	void svFloorAlertNotification(const QString& fn, Qt::GlobalColor color, int count);
	

protected:
	int watchDog;
	int watchDogLast;
	int watchDogErr;
	QTimer* pollingTimer;
	QTimer* watchDogTimer;
	QTimer* queryFaultTimer;
    void closeEvent(QCloseEvent *event);
    void paintEvent(QPaintEvent *event);

public:

	CommThread commThread;
	
	SV_Window();
	~SV_Window();

	QString floorSelectionMapName;
	void voiceAlert();
//	tPlaySound playSound;
	QThread respThread;
    QTimer respTimer;
    RespWorker respWorker;
	
	
	
	//void errorLog(QString& text);
	bool switchLogData;
	void errorLog(QString& text);

	bool connectionBreak;
	void addRtu(QString& addr,double type);
	void svAlertNotification(const QString& fn, Qt::GlobalColor color, int count);
	ConcurrentQueue<QByteArray*>	queue;
	QThread backupThread;
	QTimer backupTimer;
	
	workerBackupDb backupWorker;
	QLabel* 	displayConnectStatusWindow;
	QLabel*			displayTimeWindow;
	SV_DisplayEventStatus* 	displayEventStatusWindow;
	
	QPushButton* toolButton; 
	QMenu* menu;
	QAction* switchLogDataAction;
	QAction* exitAction;
	QAction* connectAction;
	QAction* comConfigtAction;
	QString activateMapFileName;
	//QQueue<QString> qCmdToSend;
	//QMutex muxCmdToSend;
	//QMutex muxQC_AppClosed;
	QLocalServer *cmdAgentd;
	SV_logDialog *logDlg;
	QTimer *timeTimer;
	QTimer *cmdQueueTimer;
    //QWidget window;
	QMultiMap<QString, QString> rtuMap;
	//QStringList  alertRTUs;
	QMap<QString, double>  alertRTUs;
	QMap<QString, double>  fireAlertRTUs;
	
	QMap<QString,int>	redAlertMap;
	QMap<QString,int>	greenAlertMap;
	QMap<QString,int>	blueAlertMap;
	QMap<QString,int>	yellowAlertMap;
	QMap<QString,int>	magentaAlertMap;

	int magentaAlarm;
	int redAlarm;
	int greenAlarm;
	int yellowAlarm;
	int blueAlarm;
	QStringList mapsFileList;

	void setMapsFileList(const QStringList& maps);
	//void loadAllMaps();
	
	void updateMap(QString& filename,AlertType);
	void openMap(QString& filename,AlertType,bool);
	void alertFire(QString& addr);
	void alertR4(QString& addr);
	void alertFault(QString& addr);

	void alertFireCheck(QString& addr);
	void alertR4Check(QString& addr);
	void alertControlCheck(QString& addr);
	void alertFaultCheck(QString& addr);
	
    void eliminateAlert(QString& addr,Qt::GlobalColor);
	void stopAlert();
    void setRtuMap(const QMultiMap<QString, QString>&);
    void enCmdQueue(QString var);
	//void deCmdQueue();
	//bool checkCmdQueueEmpty();
    void setMapWindow(QC_ApplicationWindow *w){
		mapWindow = w;
	} 

	void setProjectName(QString& name);
	void notifyMapWindowClosed();
    void init();
	void setConnectStatus(bool status);
	void setCurrentTime(const QString&);    

	void updateAlertReport(const QString&,Qt::GlobalColor,int);
	void initAlertReport();
	void updateRedReport(int);
	void updateMagentaReport(int);
	void updateGreenReport(int);
	void updateBlueReport(int);
	void updateYellowReport(int);
    void setActive();

	//communication with HRN
	////////////////////////////////////////////
	QMutex mutexReadData;
	char DC1;
	char STX;
	char ETX;
    char EOT;
	char ENQ;
	char ACK;
	char NAK;
	char POL;
	char cmdEOT[2];
	char cmdACK[2];
	
    QByteArray serialBuf;
	//QSerialPort *serial;
    Mgwip *settings;

	void setComConnectStatus(bool status);
	
	
	void addEventLog(Qt::GlobalColor color, const QString&, const QString& txt);
	void addEventLogItem(Qt::GlobalColor color, const QString&, const QString& txt);
	void eliminateEventLogItem(Qt::GlobalColor color,const QString& address, const QString& txt);
	void eliminateFireLogItem();
	void addDbItem(Qt::GlobalColor color, const QString& address, const QString& txt);
	void initToHRN();
	bool checkBCC(QByteArray& cmdTxt);
    void parseResp(QByteArray& resp);
	void handleCmd(QByteArray& cmd);
	void handleCmdB(QByteArray& cmd);
	void handleCmdC(QByteArray& cmd);
	void handleCmdF(QByteArray& cmd);
	void handleCmdJ(QByteArray& cmd);
	void handleCmdK(QByteArray& cmd);
	void handleCmdN(QByteArray& cmd);
	void handleCmdP(QByteArray& cmd);
	void handleCmdQ(QByteArray& cmd);
	void handleCmdR(QByteArray& cmd);
	void handleCmdS(QByteArray& cmd);
	void handleCmdT(QByteArray& cmd);
	void handleCmdV(QByteArray& cmd);
	void handleCmdZ(QByteArray& cmd);
	bool checkMultiCmds(QByteArray& txt);
	QList<QByteArray*> partCmds(QByteArray txt);

    QStringList eventGroup = {
"event_group_loop",
"event_group_fire",
"event_group_pre_alarm",
"event_group_device_all",
"event_group_earth",
"event_group_power",
"event_group_ce",
"event_group_io_mod",
"event_group_network",
"event_group_zone",
"event_group_nogroup",
"event_group_calibration",
"event_group_virtual_net",
"event_group_user"
};

QStringList eventType = {
"event_type_fire",
"event_type_evacuate",
"event_type_alert",
"event_type_pre_alarm",
"event_type_security",
"event_type_fault",
"event_type_disablement",
"event_type_tech_alarm",
"event_type_test",
"event_type_status",
"event_type_ceaction",
"event_type_none",
"event_type_max",
"event_type_other",
"event_type_all"
};

QStringList AddrType = {
"addr_type_sub_address",
"addr_type_unexpected_device",
"addr_type_common",
"addr_type_panel",
"addr_type_zone",
"addr_type_loop",
"addr_type_pio_input",
"addr_type_pio_output",
"addr_type_io_channel",
"addr_type_io_module",
"addr_type_none",
"addr_type_ce",
"addr_type_sounder",
"addr_type_back_light",
"addr_type_group",
"addr_type_timer"
};

QStringList eventCode = {
"event_none",
"event_ps_fault",
"event_calibration_fault",
"event_output_1_open_fault",
"event_output_1_short_fault",
"event_output_2_open_fault",
"event_output_2_short_fault",
"event_input_open_fault",
"event_input_short_fault",
"event_internal_fault",
"event_maintenance_fault",
"event_detector_fault",
"event_slave_open_fault",
"event_slave_short_fault",
"event_slave_1_short_fault",
"event_slave_2_short_fault",
"event_disconnected_fault",
"event_double_address_fault",
"event_monitored_output_fault",
"event_unknown_device_fault",
"event_unexpected_device_fault",
"event_wrong_device_fault",
"event_initialising_device",
"event_start",
"event_autolearn",
"event_pc_config",
"event_earth_fault",
"event_loop_wiring_fault",
"event_loop_short_cct_fault",
"event_loop_open_cct_faul",
"event_mains_failed_fault",
"event_low_battery_fault",
"event_battery_disconnected_fault",
"event_battery_overcharge_fault",
"event_aux_24v_fuse_fault",
"event_charger_fault",
"event_rom_fault",
"event_ram_fault",
"event_watch_dog_operated",
"event_bad_data_fault",
"event_unknown_event_fault",
"event_modem_active",
"event_printer_fault",
"event_en54_version_fault",
"event_pre_alarm",
"event_calibration_failed_fault",
"event_modem_fault",
"event_init_device",
"event_input_activated",
"event_optical_element_fault",
"event_heat_element_fault",
"event_both_element_fault",
"event_self_test_failed_fault",
"event_ce_active",
"event_loop_protocol_fault",
"event_loop_missing",
"event_loop_unexpected",
"event_sub_address_limit",
"event_io_mod_missing",
"event_io_mod_unexpected",
"event_serial_input",
"event_net_unexpected_node",
"event_net_unknown_type",
"event_net_missing_node",
"event_net_unexpected_card",
"event_net_missing_card",
"event_net_wrong_address",
"event_net_broken",
"event_net_comms_fault",
"event_net_comms_timeout",
"event_net_invalid_address",
"event_sounder_board_unexpected",
"event_relay_board_unexpected",
"event_sounder_board_missing",
"event_relay_board_missing",
"event_zone_io_unexpected",
"event_zone_io_missing",
"event_system_fault",
"event_disable_device",
"event_disable_zone",
"event_disable_loop",
"event_disable_sounders",
"event_disable_panel_input",
"event_disable_panel_output",
"event_disable_ce",
"event_disable_buzzer",
"event_disable_printer",
"event_disable_earth_fault",
"event_day_night_disable",
"event_general_disablement",
"event_oem_device",
"event_test",
"event_zone_io_unexpected_usa",
"event_zone_io_missing_usa",
"event_disable_immediate_output",
"event_memory_write_enable_on",
"event_annun_missing",
"event_annun_unexpected",
"event_lcd_power_fault",
"event_module_power_supply_fault",
"event_output_short_fault",
"event_output_open_fault",
"event_addressing",
"event_auto_addressing_failure",
"event_dev_battery_low",
"event_dev_tamper_fault",
"event_dev_ext_interference",
"event_dev_fata_fault",
"event_isolator_open",
"event_micro_processor_fault",
"event_prism_reflector_trgetting",
"event_alignment_mode",
"event_high_speed_fault",
"event_contamination_reached",
"event_audio_fault",
"event_head_missing_fault",
"event_tamper_fault",
"event_signal_strenght_fault",
"event_rad_battery_fault",
"event_sounder_missing_fault",
"event_dev_back_battery_low",
"event_slave_exp_loss",
"event_8zone_mimic_missing",
"event_8zone_mimic_unexpected",
"event_16zone_mimic_missing",
"event_16zone_mimic_unexpected",
"event_batt_imp_failed",
"event_aerial_tamper_fault",
"event_back_ground_out_of_range",
"event_head_fault",
"event_head_dirty_compensation",
"event_tamper_input_fault",
"event_receiver_fault",
"event_battery_fault",
"event_fuse_trip",
"event_current_limit_fault",
"event_voltage_limit_fault",
"event_weak_open_circuit",
"event_weak_short_circuit",
"event_open_circuit_fault",
"event_short_circuit_fault",
"event_board_a_missing",
"event_board_b_missing",
"event_loop_comms_timeout",
"event_all_output_disabled",
"event_all_sounders_disabled",
"event_all_zone_disabled",
"event_loop_prim_under_voltage",
"event_loop_sec_under_voltage",
"event_loop_board_missing",
"event_loop_board_unexpected",
"event_psu_earth_fault",
"event_extinguishant_activated",
"event_psu_fault",
"event_user_logged_in",
"event_autolearn_device",
"event_class_wiring_fault",
"event_ifam_missing",
"event_communicator_missing",
"event_comms_failure",
"event_comms_restored",
"event_vnet_trouble",
"event_vnet_open",
"event_vnet_shorted",
"event_vnet_restored",
"event_vnet_trans_failure",
"event_vnet_node_missing",
"event_vnet_extra_node",
"event_vnet_trans_restored",
"event_lan_not_connected",
"event_lan_net_not_recognised",
"event_lan_gateway_access_fail",
"event_lan_to_dc_comms_fail",
"event_lan_to_dc_comms_restored",
"event_dc_comms_failure",
"event_dc_comms_restored",
"event_phone_line_1_trouble",
"event_phone_line_1_restored",
"event_phone_line_2_trouble",
"event_phone_line_2_restored",
"event_verification",
"event_all_plant_output_disabled",
"event_event_log_cleared",
"event_bootloader_update",
"event_bootloader_failed",
"event_delay_extended",
"event_disable_module_io_channel",
"event_missing_io_mod_taktis_sounder",
"event_missing_io_mod_taktis_zone",
"event_missing_io_mod_taktis_relay",
"event_missing_io_mod_taktis_multi_io",
"event_unexpected_io_mod_taktis_sounder",
"event_unexpected_io_mod_taktis_zone",
"event_unexpected_io_mod_taktis_relay",
"event_unexpected_io_mod_taktis_multi_io",
"event_mgw_act1_coms_trouble",
"event_mgw_act1_conf_trouble",
"event_mgw_act2_coms_trouble",
"event_mgw_act2_conf_trouble",
"event_mgw_act3_coms_trouble",
"event_mgw_act3_conf_trouble",
"event_mgw_act4_coms_trouble",
"event_mgw_act4_conf_trouble",
"event_mgw_ipnet_conf_trouble",
"event_mgw_ipnet_coms_trouble",
"event_mgw_internal_trouble",
"event_mgw_missing",
"event_mgw_disabled",
"event_network_output_partial_short_circuit_fault",
"event_network_output_partial_open_circuit_fault",
"event_network_output_full_short_circuit_fault",
"event_network_output_full_open_circuit_fault",
"event_network_output_connection_fault",
"event_network_output_communication_fault",
"event_network_input_partial_short_circuit_fault",
"event_network_input_partial_open_circuit_fault",
"event_network_input_full_short_circuit_fault",
"event_network_input_full_open_circuit_fault",
"event_network_input_connection_fault",
"event_network_input_communication_fault",
"event_network_missing_nodes",
"event_network_connection_fault",
"event_network_repeat_address",
"event_led_missing_board",
"event_missing_io_mod_fan",
"event_missing_io_mod_ancillery",
"event_missing_io_mod_led",
"event_unexpected_io_mod_fan",
"event_unexpected_io_mod_ancillery",
"event_unexpected_io_mod_led",
"event_test_on_outpu",
"event_test_on_led",
"event_test_on_isolator",
"event_storage_inserted",
"event_monitored_input_fault",
"event_import_read",
"event_import_write",
"event_export_write",
"event_mgw_unexpected"
};

QStringList InputAction {
"action_none",
"action_fire",
"action_fault",
"action_pre_alarm",
"action_tech_alarm",
"action_evacuate",
"action_alert",
"action_security",
"action_silence",
"action_reset",
"action_transparent",
"action_disablement",
"action_test",
"action_resound",
"action_dummy",
"action_buzzer_silence",
"action_switch_on_delay",
"action_change_sens_mode",
"action_status",
"action_fire_drill",
"action_aaf",
"action_max"
};
	
	QMap<QByteArray, int> cmdContext = {
			{"B0",14},
			{"B1",14},
			{"C0",33},
			{"C1",29},
			{"C2",33},
			{"D0",29},
			{"F0",33},
			{"F3",33},
			{"J0",33},
			{"J1",33},
			{"K0",18},
			{"N0",18},
			{"N1",18},
			{"N5",18},
			{"P0",18},
			{"Q0",2},
			{"Q1",69},
			{"Q2",31},
			{"Q4",202},
			{"Q6",41},
			{"Q8",252},
			{"Q9",254},
			{"R0",10},
			{"R1",16},
			{"S0",12},
			{"S2",12},
			{"T0",33},
			{"V0",21},
			{"Z0",21},
		};
	
	QStringList B0_CODE = {
			tr("B0_CODE_001"),
			tr("B0_CODE_002"),
			tr("B0_CODE_003"),
			tr("B0_CODE_004"),
			tr("B0_CODE_005"),
			tr("B0_CODE_006"),
			tr("B0_CODE_007"),
			tr("B0_CODE_008"),
			tr("B0_CODE_009"),
			tr("B0_CODE_010"),
			tr("B0_CODE_011"),
			tr("B0_CODE_012"),
			tr("B0_CODE_013"),
			tr("B0_CODE_014"),
			tr("B0_CODE_015"),
			tr("B0_CODE_016"),
			tr("B0_CODE_017"),
			tr("B0_CODE_018"),
			tr("B0_CODE_019"),
			tr("B0_CODE_020"),
			tr("B0_CODE_021"),
			tr("B0_CODE_022"),
			tr("B0_CODE_023"),
			tr("B0_CODE_024"),
			tr("B0_CODE_025"),
			tr("B0_CODE_026"),
			tr("B0_CODE_027"),
			tr("B0_CODE_028"),
			tr("B0_CODE_029"),
			tr("B0_CODE_030"),
			tr("B0_CODE_031"),
			tr("B0_CODE_032"),
			tr("B0_CODE_033"),
			tr("B0_CODE_034"),
			tr("B0_CODE_035"),
			tr("B0_CODE_036"),
			tr("B0_CODE_037"),
			tr("B0_CODE_038"),
			tr("B0_CODE_039"),
			tr("B0_CODE_040"),
			tr("B0_CODE_041"),
			tr("B0_CODE_042"),
			tr("B0_CODE_043"),
			tr("B0_CODE_044"),
			tr("B0_CODE_045"),
			tr("B0_CODE_046"),
			tr("B0_CODE_047"),
			tr("B0_CODE_048"),
			tr("B0_CODE_049"),
			tr("B0_CODE_050"),
			tr("B0_CODE_051"),
			tr("B0_CODE_052"),
			tr("B0_CODE_053"),
			tr("B0_CODE_054"),
			tr("B0_CODE_055"),
			tr("B0_CODE_056"),
			tr("B0_CODE_057"),
			tr("B0_CODE_058"),
			tr("B0_CODE_059"),
			tr("B0_CODE_060"),
			tr("B0_CODE_061"),
			tr("B0_CODE_062"),
			tr("B0_CODE_063"),
		};
	QStringList C0_RTU_TYPE = {
			tr("C0_RTU_TYPE_a"),
			tr("C0_RTU_TYPE_b"),
			tr("C0_RTU_TYPE_c"),
			tr("C0_RTU_TYPE_d"),
			tr("C0_RTU_TYPE_e"),
			tr("C0_RTU_TYPE_f"),
			tr("C0_RTU_TYPE_g"),
			tr("C0_RTU_TYPE_h"),
			tr("C0_RTU_TYPE_i"),
			tr("C0_RTU_TYPE_j"),
			tr("C0_RTU_TYPE_k"),
			tr("C0_RTU_TYPE_l"),
			tr("C0_RTU_TYPE_m"),
			tr("C0_RTU_TYPE_n"),
			tr("C0_RTU_TYPE_o"),
			tr("C0_RTU_TYPE_p"),
			tr("C0_RTU_TYPE_q"),
			tr("C0_RTU_TYPE_r"),
			tr("C0_RTU_TYPE_s"),
			tr("C0_RTU_TYPE_t"),
			tr("C0_RTU_TYPE_u"),
			tr("C0_RTU_TYPE_v"),
			tr("C0_RTU_TYPE_w"),
			tr("C0_RTU_TYPE_x"),
			tr("C0_RTU_TYPE_y"),
			tr("C0_RTU_TYPE_z"),
			
	};

	QStringList F0_CODE = {
        tr("F0_SW_0"),
		tr("F0_SW_1"),
		tr("F0_FireAlert"),
		tr("F0_GasAlert"),

	};
	QStringList F3_CODE = {
        tr("F3_SW_0"),
		tr("F3_SW_1"),
	};
	
	QStringList J_CODE = {
		tr("J_STATE_1"),
		tr("J_STATE_2"),
		tr("J_STATE_3"),
	};
	
	
	QStringList S0_CODE = {
        tr("S0_CODE_01"),
        tr("S0_CODE_02"),
        tr("S0_CODE_03"),
		tr("S0_CODE_04"),
		tr("S0_CODE_05"),
		tr("S0_CODE_06"),
		tr("S0_CODE_07"),
		tr("S0_CODE_08"),
		tr("S0_CODE_09"),
		tr("S0_CODE_10"),
		tr("S0_CODE_11"),
		tr("S0_CODE_12"),
		tr("S0_CODE_13"),
		tr("S0_CODE_14"),
		tr("S0_CODE_15"),
		tr("S0_CODE_16"),
		tr("S0_CODE_17"),
		tr("S0_CODE_18"),
		tr("S0_CODE_19"),
		tr("S0_CODE_20"),
		tr("S0_CODE_21"),
		tr("S0_CODE_22"),
		tr("S0_CODE_23"),
		tr("S0_CODE_24"),
		tr("S0_CODE_25"),
		tr("S0_CODE_26"),
	};

	QStringList S2_CODE = {
        tr("S2_CODE_01"),
        tr("S2_CODE_02"),
        tr("S2_CODE_03"),
		tr("S2_CODE_04"),
		tr("S2_CODE_05"),
		tr("S2_CODE_06"),
		tr("S2_CODE_07"),
		tr("S2_CODE_08"),
		tr("S2_CODE_09"),
	};

	QStringList Q1_CODE = {
		tr("Q1_CODE_01"),
		tr("Q1_CODE_02"),
		tr("Q1_CODE_03"),
		tr("Q1_CODE_04"),
		tr("Q1_CODE_05"),
		tr("Q1_CODE_06"),
		tr("Q1_CODE_07"),
		tr("Q1_CODE_08"),
		tr("Q1_CODE_09"),
		tr("Q1_CODE_10"),
		tr("Q1_CODE_11"),
		tr("Q1_CODE_12"),
		tr("Q1_CODE_13"),
		tr("Q1_CODE_14"),
		tr("Q1_CODE_15"),
		tr("Q1_CODE_16"),
		tr("Q1_CODE_17"),
		tr("Q1_CODE_18"),
		tr("Q1_CODE_19"),
		tr("Q1_CODE_20"),
		tr("Q1_CODE_21"),
		tr("Q1_CODE_22"),
		tr("Q1_CODE_23"),
		tr("Q1_CODE_24"),
		tr("Q1_CODE_25"),
		tr("Q1_CODE_26"),
		tr("Q1_CODE_27"),
		tr("Q1_CODE_28"),
		tr("Q1_CODE_29"),
		tr("Q1_CODE_30"),
		tr("Q1_CODE_31"),
		tr("Q1_CODE_32"),
		tr("Q1_CODE_33"),
		tr("Q1_CODE_34"),
		tr("Q1_CODE_35"),
		tr("Q1_CODE_36"),
		tr("Q1_CODE_37"),
		tr("Q1_CODE_38"),
		tr("Q1_CODE_39"),
		tr("Q1_CODE_40"),
	};
	
	QStringList Q2_CODE = {
			tr("Q2_CODE_01"),
			tr("Q2_CODE_02"),
			tr("Q2_CODE_03"),
			tr("Q2_CODE_04"),
			tr("Q2_CODE_05"),
			tr("Q2_CODE_06"),
			tr("Q2_CODE_07"),
			tr("Q2_CODE_08"),
			tr("Q2_CODE_09"),
			tr("Q2_CODE_10"),
			tr("Q2_CODE_11"),
			tr("Q2_CODE_12"),
			tr("Q2_CODE_13"),
			tr("Q2_CODE_14"),
			tr("Q2_CODE_15"),
			tr("Q2_CODE_16"),
			tr("Q2_CODE_17"),
			tr("Q2_CODE_18"),
			tr("Q2_CODE_19"),
			tr("Q2_CODE_20"),
			tr("Q2_CODE_21"),
			tr("Q2_CODE_22"),
			tr("Q2_CODE_23"),
			tr("Q2_CODE_24"),
			tr("Q2_CODE_25"),
			tr("Q2_CODE_26"),
			tr("Q2_CODE_27"),
			tr("Q2_CODE_28"),
			tr("Q2_CODE_29"),
			
		};

	QStringList Q4_CODE = {
		tr("Q4_CODE_001"),
		tr("Q4_CODE_002"),
		tr("Q4_CODE_003"),
		tr("Q4_CODE_004"),
		tr("Q4_CODE_005"),
		tr("Q4_CODE_006"),
		tr("Q4_CODE_007"),
		tr("Q4_CODE_008"),
		tr("Q4_CODE_009"),
		tr("Q4_CODE_010"),
		tr("Q4_CODE_011"),
		tr("Q4_CODE_012"),
		tr("Q4_CODE_013"),
		tr("Q4_CODE_014"),
		tr("Q4_CODE_015"),
		tr("Q4_CODE_016"),
		tr("Q4_CODE_017"),
		tr("Q4_CODE_018"),
		tr("Q4_CODE_019"),
		tr("Q4_CODE_020"),
		tr("Q4_CODE_021"),
		tr("Q4_CODE_022"),
		tr("Q4_CODE_023"),
		tr("Q4_CODE_024"),
		tr("Q4_CODE_025"),
		tr("Q4_CODE_026"),
		tr("Q4_CODE_027"),
		tr("Q4_CODE_028"),
		tr("Q4_CODE_029"),
		tr("Q4_CODE_030"),
		tr("Q4_CODE_031"),
		tr("Q4_CODE_032"),
		tr("Q4_CODE_033"),
		tr("Q4_CODE_034"),
		tr("Q4_CODE_035"),
		tr("Q4_CODE_036"),
		tr("Q4_CODE_037"),
		tr("Q4_CODE_038"),
		tr("Q4_CODE_039"),
		tr("Q4_CODE_040"),
		tr("Q4_CODE_041"),
		tr("Q4_CODE_042"),
		tr("Q4_CODE_043"),
		tr("Q4_CODE_044"),
		tr("Q4_CODE_045"),
		tr("Q4_CODE_046"),
		tr("Q4_CODE_047"),
		tr("Q4_CODE_048"),
		tr("Q4_CODE_049"),
		tr("Q4_CODE_050"),
		tr("Q4_CODE_051"),
		tr("Q4_CODE_052"),
		tr("Q4_CODE_053"),
		tr("Q4_CODE_054"),
		tr("Q4_CODE_055"),
		tr("Q4_CODE_056"),
		tr("Q4_CODE_057"),
		tr("Q4_CODE_058"),
		tr("Q4_CODE_059"),
		tr("Q4_CODE_060"),
		tr("Q4_CODE_061"),
		tr("Q4_CODE_062"),
		tr("Q4_CODE_063"),
		tr("Q4_CODE_064"),
		tr("Q4_CODE_065"),
		tr("Q4_CODE_066"),
		tr("Q4_CODE_067"),
		tr("Q4_CODE_068"),
		tr("Q4_CODE_069"),
		tr("Q4_CODE_070"),
		tr("Q4_CODE_071"),
		tr("Q4_CODE_072"),
		tr("Q4_CODE_073"),
		tr("Q4_CODE_074"),
		tr("Q4_CODE_075"),
		tr("Q4_CODE_076"),
		tr("Q4_CODE_077"),
		tr("Q4_CODE_078"),
		tr("Q4_CODE_079"),
		tr("Q4_CODE_080"),
		tr("Q4_CODE_081"),
		tr("Q4_CODE_082"),
		tr("Q4_CODE_083"),
		tr("Q4_CODE_084"),
		tr("Q4_CODE_085"),
		tr("Q4_CODE_086"),
		tr("Q4_CODE_087"),
		tr("Q4_CODE_088"),
		tr("Q4_CODE_089"),
		tr("Q4_CODE_090"),
		tr("Q4_CODE_091"),
		tr("Q4_CODE_092"),
		tr("Q4_CODE_093"),
		tr("Q4_CODE_094"),
		tr("Q4_CODE_095"),
		tr("Q4_CODE_096"),
		tr("Q4_CODE_097"),
		tr("Q4_CODE_098"),
		tr("Q4_CODE_099"),
		tr("Q4_CODE_100"),
		tr("Q4_CODE_101"),
		tr("Q4_CODE_102"),
		tr("Q4_CODE_103"),
		tr("Q4_CODE_104"),
		tr("Q4_CODE_105"),
		tr("Q4_CODE_106"),
		tr("Q4_CODE_107"),
		tr("Q4_CODE_108"),
		tr("Q4_CODE_109"),
		tr("Q4_CODE_110"),
		tr("Q4_CODE_111"),
		tr("Q4_CODE_112"),
		tr("Q4_CODE_113"),
		tr("Q4_CODE_114"),
		tr("Q4_CODE_115"),
		tr("Q4_CODE_116"),
		tr("Q4_CODE_117"),
		tr("Q4_CODE_118"),
		tr("Q4_CODE_119"),
		tr("Q4_CODE_120"),
		tr("Q4_CODE_121"),
		tr("Q4_CODE_122"),
		tr("Q4_CODE_123"),
		tr("Q4_CODE_124"),
		tr("Q4_CODE_125"),
		tr("Q4_CODE_126"),
		tr("Q4_CODE_127"),
		tr("Q4_CODE_128"),
		tr("Q4_CODE_129"),
		tr("Q4_CODE_130"),
		tr("Q4_CODE_131"),
		tr("Q4_CODE_132"),
		tr("Q4_CODE_133"),
		tr("Q4_CODE_134"),
		tr("Q4_CODE_135"),
		tr("Q4_CODE_136"),
		tr("Q4_CODE_137"),
		tr("Q4_CODE_138"),
		tr("Q4_CODE_139"),
		tr("Q4_CODE_140"),
		tr("Q4_CODE_141"),
		tr("Q4_CODE_142"),
		tr("Q4_CODE_143"),
		tr("Q4_CODE_144"),
		tr("Q4_CODE_145"),
		tr("Q4_CODE_146"),
		tr("Q4_CODE_147"),
		tr("Q4_CODE_148"),
		tr("Q4_CODE_149"),
		tr("Q4_CODE_150"),
		tr("Q4_CODE_151"),
		tr("Q4_CODE_152"),
		tr("Q4_CODE_153"),
		tr("Q4_CODE_154"),
		tr("Q4_CODE_155"),
		tr("Q4_CODE_156"),
		tr("Q4_CODE_157"),
		tr("Q4_CODE_158"),
		tr("Q4_CODE_159"),
		tr("Q4_CODE_160"),
		tr("Q4_CODE_161"),
		tr("Q4_CODE_162"),
		tr("Q4_CODE_163"),
		tr("Q4_CODE_164"),
		tr("Q4_CODE_165"),
		tr("Q4_CODE_166"),
		tr("Q4_CODE_167"),
		tr("Q4_CODE_168"),
		tr("Q4_CODE_169"),
		tr("Q4_CODE_170"),
		tr("Q4_CODE_171"),
		tr("Q4_CODE_172"),
		tr("Q4_CODE_173"),
		tr("Q4_CODE_174"),
		tr("Q4_CODE_175"),
		tr("Q4_CODE_176"),
		tr("Q4_CODE_177"),
		tr("Q4_CODE_178"),
		tr("Q4_CODE_179"),
		tr("Q4_CODE_180"),
		tr("Q4_CODE_181"),
		tr("Q4_CODE_182"),
		tr("Q4_CODE_183"),
		tr("Q4_CODE_184"),
		tr("Q4_CODE_185"),
		tr("Q4_CODE_186"),
		tr("Q4_CODE_187"),
		tr("Q4_CODE_188"),
		tr("Q4_CODE_189"),
		tr("Q4_CODE_190"),
		tr("Q4_CODE_191"),
		tr("Q4_CODE_192"),
		tr("Q4_CODE_193"),
		tr("Q4_CODE_194"),
		tr("Q4_CODE_195"),
		tr("Q4_CODE_196"),
		tr("Q4_CODE_197"),
		tr("Q4_CODE_198"),
		tr("Q4_CODE_199"),
		tr("Q4_CODE_200"),
	};
	///////////////////////////////////////////////

public slots:
    void runTest1();
    void runTest2();
    void runTest3();
    void runTest4();
    void runTest5();
    void runTest6();
    void runTest7();
    void runTest8();
	void connectCheck(const TypeConnectStatus &s);
	//communication with HRN
	////////////////////////////////////////////
	void queryFaultReportHRN();
	void configureCom();
	void openSerialPort();
	void closeSerialPort();
	void readData(const TypeMceEventLog &s);
	////////////////////////////////////////////
	void recvCmd();
	void slotTimer();
    void slotComConnect();
	void slotSwitchLogData();
	void slotComConfig();
	void slotExit();
	void slotEventLog();
	void slotViewMap();
	void slotResetHRN();
	void slotSwitchMainAlarm();
    void slotCheckMap();
	void slotQC_AppWindowClosed();
	void slotAutoPrint();
	void slotPrintPreviewChanged(bool state);
	void slotBackupDb();
	void slotLwDoubleClicked(QListWidgetItem*);
	void slotCmdDequeue();
	void slotWatchDog();
	//////////////////////////////
	//slots for commthread 
	
    void commError(const QString &s);
    void commTimeout(const QString &s);
	void getFloorSelectionMapName(const QString &s);
	
	

private:
	void connectionAttach();
	void connectionDetatch();
	void createMapWindow();
	void closeMapWindow();
	//SV_Label *windowTitle;
	QLabel *windowTitle;
	QString projectName;
	
	QC_ApplicationWindow* mapWindow;

	QPushButton *autoPrintBtn;
	//QPushButton *connectBtn;
	//QPushButton *setupBtn;
	//QPushButton *exitBtn;
	//QLabel		*connectStatus;
	//QLabel 		*currentTime;
	QLabel 		*eventMessage;
	QLabel		*summary;
	
	QListWidget *eventList;
	QPushButton *eventLogBtn;
	QPushButton *viewMapBtn;
	QPushButton *resetBtn;
	QPushButton *mainAlarmBtn;
	QPushButton *checkMapBtn;
	
};


#endif 

