Name:		pobvnc
Version:	0.2
Release:	1%{?dist}
Summary:	Pobvnc is an application that enables you to help other people behind a firewall.
Group:		Network
License:	GPL
URL:		http://www.freemedialab.org/pobvnc/
Source0:	%{name}-master.zip

BuildRequires:	cmake gtk2-devel
Requires:	gtk2 x11vnc tigervnc

%description
Pobvnc is an application that enables you to help other people behind a firewall.

%prep
%autosetup -n pobvnc-master


%build
cmake .
make 

%install
%make_install

%files
/usr/bin/pobvnc
/usr/share/applications/pobvnc.desktop
/usr/share/icons/lifesaver.svg
