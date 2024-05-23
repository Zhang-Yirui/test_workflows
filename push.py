import requests

url = "https://sctapi.ftqq.com/SCT133543TckfLrr2C2A9T6jQTro1bsFoL.send"
data = {
    "title": "Github Action push test",
    "desp": "此条消息来自github action的推送",
    "channel": "18"
}

response = requests.post(url, json=data)

if response.status_code == 200:
    print('Success:', response.json())
else:
    print('Failed:', response.status_code, response.text)
