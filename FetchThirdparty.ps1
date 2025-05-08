param (
    [switch]$UseProxy = $false,
    [int]$ProxyPort = 10809
)

$zipUrl = "https://github.com/qihaichiaki/Namica/releases/download/ThirdParty/thirdparty.zip"
$zipPath = "$PSScriptRoot\thirdparty.zip"
$extractPath = "$PSScriptRoot\thirdparty"

# 删除已有文件夹
if (Test-Path $extractPath) {
    Write-Host "Removing existing folder: $extractPath"
    Remove-Item $extractPath -Recurse -Force
}

# 下载文件
Write-Host "Downloading zip file..."
try {
    if ($UseProxy) {
        $proxyAddress = "http://127.0.0.1:$ProxyPort"
        Write-Host "Using proxy: $proxyAddress"
        Invoke-WebRequest -Uri $zipUrl -OutFile $zipPath -Proxy $proxyAddress
    } else {
        Write-Host "Not using proxy."
        Invoke-WebRequest -Uri $zipUrl -OutFile $zipPath
    }
} catch {
    Write-Error "Download failed: $_"
    exit 1
}

# 解压并删除
if (Test-Path $zipPath) {
    Write-Host "Extracting..."
    Expand-Archive -Path $zipPath -DestinationPath $extractPath -Force

    Write-Host "Cleaning up zip file..."
    Remove-Item $zipPath -Force
    Write-Host "Done."
} else {
    Write-Error "Downloaded zip file not found: $zipPath"
    exit 1
}
