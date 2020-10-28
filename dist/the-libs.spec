Name:           the-libs
Version:        3.0
Release:        1%{?dist}
Summary:        Common libraries for the* apps

License:        GPLv3+
URL:            https://github.com/vicr123/the-libs
Source0:        https://github.com/vicr123/the-libs/archive/v%{version}.tar.gz

BuildRequires:  make qt5-devel
Requires:       qt5

%define debug_package %{nil}
%define _unpackaged_files_terminate_build 0

%description
Common libraries for the* apps

%package        devel
Summary:        Development files for %{name}
Requires:       %{name}%{?_isa} = %{version}-%{release}

%description    devel
The %{name}-devel package contains libraries and header files for
developing applications that use %{name}.


%prep
%setup

%build
qmake-qt5
make

%install
rm -rf $RPM_BUILD_ROOT
#%make_install
make install INSTALL_ROOT=$RPM_BUILD_ROOT
find $RPM_BUILD_ROOT -name '*.la' -exec rm -f {} ';'


%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig


%files
%{_libdir}/*.so.*

%files devel
%{_includedir}/*
%{_libdir}/*.so
%{_libdir}/qt5/mkspecs/modules/qt_thelib.pri
%{_datadir}/the-libs/pri/*.pri


%changelog
* Sun May  3 2020 Victor Tran
- 
