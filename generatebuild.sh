#!/usr/bin/env bash

mkdir Server/Licenses
cp Install/ThirdPartyLicenses/* Server/Licenses/
cp CONTRIBUTORS Server/
cp LICENSE Server/
tar -cvzf Cuberite.tar.gz Server/*

mkdir build
mv Cuberite.tar.gz build/
