Name:           the-libs-blueprint
Version:        3.0
Release:        1%{?dist}
Summary:        Common libraries for the* apps

License:        GPLv3+
URL:            https://github.com/vicr123/the-libs
Source0:        https://github.com/vicr123/the-libs/archive/v%{version}.tar.gz
Conflicts:      the-libs
Provides:       the-libs

%if 0%{?fedora} == 32
BuildRequires:  make qt5-devel libunwind-devel gsettings-qt-devel
Requires:       qt5 libunwind gsettings-qt
%endif

%if 0%{?fedora} >= 33
BuildRequires:  make qt5-qtbase-devel qt5-qtmultimedia-devel qt5-qtsvg-devel qt5-qtx11extras-devel libunwind-devel gsettings-qt-devel qt5-linguist
Requires:       qt5-qtbase qt5-qtmultimedia qt5-qtsvg qt5-qtx11extras libunwind gsettings-qt
%endif

%define debug_package %{nil}
%define _unpackaged_files_terminate_build 0

%description
Common libraries for the* apps

%package        devel
Summary:        Development files for %{name}
Requires:       %{name}%{?_isa} = %{version}-%{release}
Conflicts:      the-libs-devel
Provides:       the-libs-devel

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
mkdir -p $RPM_BUILD_ROOT/usr/lib64/qt5/mkspecs/modules/
mv $RPM_BUILD_ROOT/../lib64/qt5/mkspecs/modules/qt_thelib.pri $RPM_BUILD_ROOT/usr/lib64/qt5/mkspecs/modules/

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig


%files
%{_libdir}/*.so.*

%files devel
%{_includedir}/*
%{_libdir}/*.so
%{_libdir}/qt5/mkspecs/modules/qt_thelib.pri
%{_datadir}/the-libs/pri/*.pri
%{_bindir}/tltrdesktop


%changelog
* Sun May  3 2020 Victor Tran
- 
