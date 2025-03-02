@echo off
echo Setting up Python virtual environment...

:: Create a virtual environment in the 'venv' folder
python -m venv venv

:: Activate the virtual environment
call venv\Scripts\activate

:: Upgrade pip to the latest version
python -m pip install --upgrade pip

:: Install required packages
if exist requirements.txt (
    echo Installing dependencies from requirements.txt...
    pip install -r requirements.txt
) else (
    echo requirements.txt not found. Please create one with necessary dependencies.
)

echo Virtual environment setup complete!
echo To activate it later, run: venv\Scripts\activate