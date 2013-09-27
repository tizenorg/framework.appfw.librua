Name:       librua
Summary:    Recently used application
Version:    0.1.2
Release:    1
Group:      Application Framework/Libraries
License:    Apache-2.0
Source0:    librua-%{version}.tar.gz
Requires(post): /sbin/ldconfig
Requires(post): /usr/bin/sqlite3
Requires(postun): /sbin/ldconfig
BuildRequires:  cmake
BuildRequires:  pkgconfig(sqlite3)
BuildRequires:  pkgconfig(db-util)

%description
Recently used application library

%package devel
Summary:    Recently used application (devel)
Group:      Development/Libraries
Requires:   %{name} = %{version}-%{release}

%description devel
Recently used application library (devel)

%prep
%setup -q

%build
%cmake .

make %{?jobs:-j%jobs}

%install
%make_install
mkdir -p %{buildroot}/usr/share/license
install LICENSE %{buildroot}/usr/share/license/%{name}

%post
/sbin/ldconfig
mkdir -p /opt/dbspace/
sqlite3 /opt/dbspace/.rua.db < /opt/share/rua_db.sql
rm -rf /opt/share/rua_db.sql
chown 0:5000 /opt/dbspace/.rua.db
chown 0:5000 /opt/dbspace/.rua.db-journal
chmod 660 /opt/dbspace/.rua.db
chmod 660 /opt/dbspace/.rua.db-journal
chsmack -a rua::db /opt/dbspace/.rua.db
chsmack -a rua::db /opt/dbspace/.rua.db-journal

%postun -p /sbin/ldconfig

%files
%manifest librua.manifest
%defattr(-,root,root,-)
%config(missingok) /opt/share/rua_db.sql
%{_libdir}/librua.so.*
/usr/share/license/%{name}

%files devel
%defattr(-,root,root,-)
/usr/include/rua/*.h
%{_libdir}/librua.so
%{_libdir}/pkgconfig/rua.pc

