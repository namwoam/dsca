FROM alpine:3.20

RUN apk add --no-cache python3 py3-pip

WORKDIR /app

RUN python3 -m venv /opt/venv
ENV PATH="/opt/venv/bin:$PATH"

RUN pip install --upgrade pip
RUN pip install requests==2.32.3

COPY client.py .

CMD ["python3", "client.py"]