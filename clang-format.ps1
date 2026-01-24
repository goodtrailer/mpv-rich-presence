clang-format -i @(Get-ChildItem -Recurse -Path @("include", "src") -Filter *.hpp*)
clang-format -i @(Get-ChildItem -Recurse -Path @("include", "src") -Filter *.cpp*)
