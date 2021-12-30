Name: netrate
Version: 0.1
Release: 1%{?dist}
Summary: Network interface traffic meter
License: GPLv2
URL: https://github.com/mindbit/netrate
Source0: https://github.com/mindbit/%{name}/archive/v%{version}/%{name}-%{version}.tar.gz
BuildRequires: gcc, make

%description
netrate is a simple program that displays real-time byte and packet
count rate of network interfaces in Linux systems.

%prep
%setup -q

%build
make CFLAGS="-g -O2" %{?_smp_mflags} -C src

%install
%make_install -C src

%files
%{_bindir}/netrate
%license LICENSE.md
%doc README.md

%changelog
* Thu Dec 30 2021 Radu Rendec <radu@rendec.net> - 0.1-1
- First release
