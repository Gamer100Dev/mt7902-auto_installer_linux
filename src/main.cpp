#include <cstdio>
#include <iostream>
#include <string>
using namespace std;
#include <sys/stat.h>
#include <fstream>
#include <filesystem>
#include <sys/utsname.h>
#include <stdio.h>
#include <unistd.h>

namespace fs = std::filesystem;
string MT7902_Firmware_Location = "/lib/firmware/mediatek/mt7902";
string Local_MT7902_FW_Location = fs::current_path().string() + "/mt7902_firmware";
string Local_MT7902_Wifi_Location = fs::current_path().string() + "/gen4-mt7902";
string current_path_Location = fs::current_path().string();
string bluetooth_package_Location = current_path_Location + "/bluetooth";
fs::path dest_dir = fs::path(MT7902_Firmware_Location);
fs::path source_dir = fs::path(Local_MT7902_FW_Location);
fs::path Local_MT7902_Wifi_Location_dir = fs::path(Local_MT7902_Wifi_Location);

string uname_r(){
    struct utsname buffer;
    if (uname(&buffer) != 0) {
        perror("uname");
        exit(1);
    }
    string kernel_type = buffer.release;
    return kernel_type;
}

string bluetooth_lib_module = "/lib/modules/" + uname_r() + "/kernel/drivers/bluetooth";

int kernel_headers_exist() {
    std::string path = "/usr/lib/modules/" + uname_r() + "/build";
    struct stat info;
    if (stat(path.c_str(), &info) != 0 || !(info.st_mode & S_IFDIR)) {
        return 1;
    }
    return 0;
}

int text_log(string text){
    cout << "LOG: " << text << endl;
    return 0;
}

int run(const string &cmd){
    int r = system(cmd.c_str());
    if (r != 0){
        text_log("Command failed: " + cmd);
        exit(1);
    }
    return r;
}

int main(){
    if (geteuid() != 0) {
        text_log("This program requires root. Please run it with sudo.");
        return 1;
    }
    int KERNEL_MODULES = kernel_headers_exist();
    if (KERNEL_MODULES == 1){
        text_log("There is no kernel modules installed! " + uname_r());
        text_log("\n /usr/lib/modules/"  + uname_r());
    }
    if (KERNEL_MODULES == 0){
        text_log("Kernel modules are present within this system! Continuing!");
        try{
            fs::create_directories("/lib/firmware/mediatek");
        } catch (fs::filesystem_error &error){
            text_log(error.what());
        }
        try{
            fs::create_directories(MT7902_Firmware_Location);
        } catch (fs::filesystem_error &error){
            text_log(error.what());
        }
        if (fs::exists(source_dir) && fs::is_directory(source_dir)){
            for (const auto& entry : fs::directory_iterator(source_dir)){
                if (entry.is_regular_file()){
                    fs::path file_path = entry.path();
                    if (file_path.extension() == ".zst"){
                        fs::path dest_root = fs::path("/lib/firmware/mediatek") / file_path.filename();
                        try {
                            fs::copy_file(file_path, dest_root, fs::copy_options::overwrite_existing);
                            text_log("Coped:" + file_path.string() + " to " + dest_root.string());
                        } catch (fs::filesystem_error& e) {
                            text_log("Error copying:" + file_path.string() + " : " + e.what());
                        }
                    } else if (file_path.extension() == ".bin"){
                        fs::path dest_mt = fs::path(MT7902_Firmware_Location) / file_path.filename();
                        try {
                            fs::copy_file(file_path, dest_mt, fs::copy_options::overwrite_existing);
                            text_log("Coped:" + file_path.string() + " to " + dest_mt.string());
                        } catch (fs::filesystem_error& e) {
                            text_log("Error copying:" + file_path.string() + " : " + e.what());
                        }
                    }
                }
            }
        } else {
            text_log("Source firmware directory does not exist: " + source_dir.string());
        }
        string command = "cd " + bluetooth_package_Location + " && make -j$(nproc)";
        run(command);
        fs::path modules_dir = fs::path(bluetooth_lib_module);
        if (!fs::exists(modules_dir)) {
            try{
                fs::create_directories(modules_dir);
            } catch (fs::filesystem_error &e){
                text_log(e.what());
            }
        }
        if (fs::exists(bluetooth_package_Location) && fs::is_directory(bluetooth_package_Location)){
            for (const auto& entry_BPL: fs::directory_iterator(bluetooth_package_Location)){
                if (entry_BPL.is_regular_file()){
                    fs::path binary_path = entry_BPL.path();
                    if (binary_path.extension() == ".ko"){
                        fs::path dest_module = modules_dir / binary_path.filename();
                        try{
                            fs::copy_file(binary_path, dest_module, fs::copy_options::overwrite_existing);
                        }catch(fs::filesystem_error &e){
                            text_log("Error copying:" + binary_path.string() + " : " + e.what());
                            exit(1);
                        }
                        string ko_name = binary_path.filename().string();
                        string firmware_compressed = MT7902_Firmware_Location + "/" + ko_name + ".zst";
                        string compress_cmd = "zstd -f -q -o '" + firmware_compressed + "' '" + dest_module.string() + "'";
                        run(compress_cmd);
                        text_log("The bluetooth module has compiled successfully and it has been installed to your system!");
                    }
                }
            }
        }
        run("depmod -a");
        text_log("Now attempting to compile the wifi drivers! Please wait..");
        string command_wifi_compile = "cd " + Local_MT7902_Wifi_Location + " && make -j$(nproc)";
        run(command_wifi_compile);
        string install_command = "cd " + Local_MT7902_Wifi_Location + " && make install -j$(nproc)";
        run(install_command);
        run("depmod -a");
        text_log("The drivers for the MT7902 has successfully been installed! Please reboot your system to apply all the changes made!");
        return 0;
    }
}
