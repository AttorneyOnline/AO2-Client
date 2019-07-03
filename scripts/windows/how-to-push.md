When you want to build a new version of Qt:
```docker
docker build -t mxe-windows-static . -f Dockerfile-mxe
docker tag mxe-windows-static oldmud0/mxe-qt:5.12.1-win32-static-posix
docker push oldmud0/mxe-qt:5.12.1-win32-static-posix
```

Remember to log into Docker Hub before attempting to push.

When you want to build a new version of any dependency required for building AO:
```docker
docker build -t mxe-windows-static-ao . -f Dockerfile
docker tag mxe-windows-static-ao registry.gitlab.com/attorneyonline/ao2-client/builder-windows-i686
docker push registry.gitlab.com/attorneyonline/ao2-client/builder-windows-i686
```

Remember to create an access token in GitLab before attempting to push.

GitLab CI depends on `builder-windows-i686` image to be present in the repository's registry in order for the Windows build to succeed. 
