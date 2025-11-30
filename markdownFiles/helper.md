.\build.ps1
.\build\strategy.exe .\data\sample.csv 10 7
.\build\strategy.exe .\data\ohlcv_100_days_style.csv 10 7


## To build the model and run
cd X:\Projects\Personal\SP
Remove-Item -Recurse -Force build -ErrorAction SilentlyContinue
mkdir build
cd build
cmake ..
cmake --build . --config Release
.\Release\predictor.exe ..\data\stock_data.csv 1 0.8

## To build and run
cd X:\Projects\Personal\SP
.\build.ps1 -Run

# OR simply build and run manually:
.\build.ps1
.\build\Release\predictor.exe data\stock_data.csv 1 0.8


## To run the model
.\build\Release\predictor.exe data\stock_data.csv 1 0.8
