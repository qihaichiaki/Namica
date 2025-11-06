import os
from pathlib import Path
import Utils

class VulkanConfiguration:
    requiredVulkanVersion = "1.4."
    installVulkanVersion = "1.4.328.1"
    vulkanDirectory = "./../namica/vendor/VulkanSDK"

    @classmethod
    def Validate(cls):
        if (not cls.CheckVulkanSDK()):
            print("`Vulkan SDK`未正确安装")
            return
            
        if (not cls.CheckVulkanSDKDebugLibs()):
            print("\n未找到`Vulkan SDK`的调试库(Debug Libs), 请重新安装`Vulkan SDK`并勾选包含调试库的选项")
            print("(参见官方文档: docs.hazelengine.com/GettingStarted).")
            print("调试配置功能已被禁用.")

    @classmethod
    def CheckVulkanSDK(cls):
        vulkanSDK = os.environ.get("VULKAN_SDK")
        if (vulkanSDK is None):
            print("\n未检测到`Vulkan SDK`!")
            cls.__InstallVulkanSDK()
            return False
        else:
            print(f"\n已检测到`Vulkan SDK`安装路径：{vulkanSDK}")

        if (cls.requiredVulkanVersion not in vulkanSDK):
            print(f"`Vulkan SDK`版本不符合要求!(引擎需要版本号前缀为: {cls.requiredVulkanVersion})")
            cls.__InstallVulkanSDK()
            return False
    
        print(f"`Vulkan SDK`版本符合要求: {vulkanSDK}")
        return True

    @classmethod
    def __InstallVulkanSDK(cls):
        permissionGranted = False
        while not permissionGranted:
            reply = str(input("是否安装`Vulkan SDK`{0:s}? [Y/N]: ".format(cls.installVulkanVersion))).lower().strip()[:1]
            if reply == 'n':
                return
            permissionGranted = (reply == 'y')

        vulkanInstallURL = f"https://sdk.lunarg.com/sdk/download/{cls.installVulkanVersion}/windows/vulkansdk-windows-X64-{cls.installVulkanVersion}.exe"
        vulkanPath = f"{cls.vulkanDirectory}/vulkansdk-windows-X64-{cls.installVulkanVersion}.exe"
        print("下载目标: {0:s} to {1:s}".format(vulkanInstallURL, vulkanPath))
        Utils.DownloadFile(vulkanInstallURL, vulkanPath)
        print("运行`Vulkan SDK`安装程序中...")
        os.startfile(os.path.abspath(vulkanPath))
        print("安装程序已启动, 请在安装完成后重新运行此脚本检查正确性!")
        quit()

    @classmethod
    def CheckVulkanSDKDebugLibs(cls):
        vulkanSDK = os.environ.get("VULKAN_SDK")
        shadercdLib = Path(f"{vulkanSDK}/Lib/shaderc_sharedd.lib")
        
        return shadercdLib.exists()

if __name__ == "__main__":
    VulkanConfiguration.Validate()