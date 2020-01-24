from flask import Flask, redirect, url_for, request
app = Flask(__name__)

@app.route("/")
def main():
    return "Hello World!"

@app.route("/updates", methods = ['POST','GET'])
def updates():
    if request.method == 'POST':
        return "USE GET NOT POST"
    elif request.method == 'GET':
        client_ver = request.args.get('cver')
        if client_ver is int:
            client_ver = int(client_ver)
            if client_ver < 23:
                return 'UPD'
            else:
                return 'IDL'
        else:
            return '500'

    else:
        return 'You suck'

if __name__ == "__main__":
    app.run()
