CSVLoader loader(csv_path);
auto bars = loader.load();  // Load historical data

FeatureEngineer engineer(config);
auto [features, targets] = engineer.create_features(bars, 1);//COnverting raw stock data into 16 features and 1 means to predict 1 day ahead

auto [train_X, train_y, test_X, test_y] = 
    engineer.train_test_split(features, targets, 0.8);//splitting data into 80/20

LinearRegression model;
model.train(train_X, train_y);  // Train on 80%

double r2 = model.r_squared(test_X, test_y);  // Evaluate on 20%