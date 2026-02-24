ARG BASE_IMAGE=cowrie/cowrie:latest
FROM ${BASE_IMAGE}

# Cowrie upstream image may not include /bin/sh or apt.
# Keep this Dockerfile as a thin wrapper so compose remains customizable
# via BASE_IMAGE without running shell-based build steps.
