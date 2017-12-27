# sampctl Pawn Package makefile

VERSION := $(shell cat VERSION)

ensurebuild:
	sampctl package build --forceEnsure

build:
	sampctl package build

release:
	git tag $(VERSION)
	git push --tags
