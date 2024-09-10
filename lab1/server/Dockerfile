FROM alpine:3.20

RUN apk add --no-cache python3 py3-pip

WORKDIR /app

RUN python3 -m venv /opt/venv
ENV PATH="/opt/venv/bin:$PATH"

RUN pip install --upgrade pip
RUN pip install flask==3.0.0

COPY server.py .

ENV FLASK_APP=server.py
ENV FLASK_RUN_HOST=0.0.0.0

CMD ["flask", "run"]
